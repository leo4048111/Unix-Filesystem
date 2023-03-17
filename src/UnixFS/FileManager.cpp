#include "FileManager.hpp"
#include "FileSystem.hpp"
#include "InodeTable.hpp"
#include "SuperBlockManager.hpp"
#include "SuperBlock.hpp"
#include "BufferManager.hpp"
#include "DirectoryEntry.hpp"
#include "DiskDriver.hpp"

#include "Log.hpp"

#include <time.h>

namespace ufs
{
    std::unique_ptr<FileManager> FileManager::_inst;

    FileManager::FileManager()
    {
    }

    FileManager::~FileManager()
    {
    }

    Error FileManager::mount()
    {
        // Steps to mount the disk
        // (1) initialize memory InodeCache with InodeCache::clearCache()
        // (2) Open the disk image file with DiskDriver::mount()
        // (3) Load SuperBlock to SuperBlock cache

        if (isMounted())
        {
            Error ec = Error::UFS_ERR_ALREADY_MOUNTED;
            return ec;
        }

        InodeTable::getInstance()->clear();

        Error ec = FileSystem::getInstance()->mount();

        if (ec == Error::UFS_NOERR)
            _isMounted = true;

        return ec;
    }

    Error FileManager::unmount()
    {
        Error ec = Error::UFS_NOERR;
        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to unmount the disk
        // (1) Flush all dirty InodeCache to disk with InodeCache::flushAllDirtyInodeCache()
        // (2) Flush SuperBlock cache to disk with SuperBlock::flushSuperBlock()
        // (3) Close the disk image file with DiskDriver::unmount()
        InodeTable::getInstance()->flushAllDirtyInodeCache();

        if (SuperBlockManager::getInstance()->isDirty())
        {
            SuperBlockManager::getInstance()->flushSuperBlockCache();
        }

        ec = FileSystem::getInstance()->unmount();

        if (ec == Error::UFS_NOERR)
            _isMounted = false;

        return ec;
    }

    Error FileManager::format()
    {
        // Steps to format the disk
        // (1) clean the disk, set all bytes to 0
        // (2) initialize SuperBlock and flush it back to disk
        // (3) initialize InodeTable and flush it back to disk
        // (4) initialize Inodes' datablocks and flush them back to disk
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        DiskDriver::getInstance()->clear();

        // basic structure for disk.img
        // | superblock |  inodes   | data blocks              |
        // |     0#     |  1# ~ 3#  | 4# ~ (DISK_BLOCK_NUM-1)# |

        // load superblock in 0# disk block first
        SuperBlock tmpSB;
        tmpSB.s_isize = 3;
        tmpSB.s_fsize = DISK_BLOCK_NUM;
        tmpSB.s_fmod = 1;
        tmpSB.s_time = time(NULL);

        // TODO Implement chain memory allocation
        for (tmpSB.s_nfree = 0; tmpSB.s_nfree < 100; tmpSB.s_nfree++)
            tmpSB.s_free[tmpSB.s_nfree] = 99 - tmpSB.s_nfree;

        // init inode table
        for (tmpSB.s_ninode = 0; tmpSB.s_ninode < 100; tmpSB.s_ninode++)
            tmpSB.s_inode[tmpSB.s_ninode] = 99 - tmpSB.s_ninode;

        // init root directory
        int superBlockDiskBlkno = tmpSB.balloc(); // alloc 0# diskblock to store superblock
        int inodepoolDiskBlkno[3] = {
            tmpSB.balloc(),
            tmpSB.balloc(),
            tmpSB.balloc()}; // aloc 1# ~ 3# diskblock to store inode pool

        // TODO basic file tree struct

        Inode tmpInode;
        tmpInode.i_mode = Inode::IFDIR;
        tmpInode.i_nlink = 1;
        tmpInode.i_count = 1;
        tmpInode.i_flag |= (Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC);
        tmpInode.i_size = 3 * sizeof(DirectoryEntry);

        // init root directory inode
        int rootDirInodeNo = tmpSB.ialloc();
        tmpInode.i_addr[0] = tmpSB.balloc();
        int rootDirDiskBlkno = tmpInode.i_addr[0];
        InodeTable::getInstance()->iupdate(rootDirInodeNo, tmpInode);

        // init root/home directory inode
        int homeDirInodeNo = tmpSB.ialloc();
        tmpInode.i_addr[0] = tmpSB.balloc();
        tmpInode.i_size = 3 * sizeof(DirectoryEntry);
        InodeTable::getInstance()->iupdate(rootDirInodeNo, tmpInode);

        // Flush all new inodes back to disk
        InodeTable::getInstance()->flushAllDirtyInodeCache();

        // TODO: possible size check
        // // using buffer to write to Datablock
        // Buf* buf = BufferManager::getInstance()->getBlk(tmpInode.i_addr[0]);

        // init DirectoryEntry structure for root dir
        DirectoryEntry *dataOffset = (DirectoryEntry *)(rootDirDiskBlkno * DISK_BLOCK_SIZE);
        DirectoryEntry tmpDirEntry;
        // . => root dir
        strcpy(tmpDirEntry._name, "."); // . should point to root dir
        tmpDirEntry._ino = rootDirInodeNo;
        DiskDriver::getInstance()->writeOffset((size_t)dataOffset, tmpDirEntry);
        dataOffset++; // update pointer to next DirectoryEntry
        // .. => root dir
        strcpy(tmpDirEntry._name, ".."); // .. should point to root dir
        tmpDirEntry._ino = rootDirInodeNo;
        DiskDriver::getInstance()->writeOffset((size_t)dataOffset, tmpDirEntry);
        dataOffset++; // update pointer to next DirectoryEntry

        // home => root/home
        strcpy(tmpDirEntry._name, "home"); // .. should point to root dir
        tmpDirEntry._ino = homeDirInodeNo;
        DiskDriver::getInstance()->writeOffset((size_t)dataOffset, tmpDirEntry);
        dataOffset++;

        // Flush SuperBlock to disk
        SuperBlockManager::getInstance()->setSuperBlock(tmpSB);
        SuperBlockManager::getInstance()->flushSuperBlockCache();

        // Flush all dirty buffers back to disk
        BufferManager::getInstance()->flush();

        return ec;
    }
}