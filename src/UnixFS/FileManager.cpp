#include "Log.hpp"

#include "FileManager.hpp"
#include "FileSystem.hpp"
#include "InodeTable.hpp"
#include "SuperBlockManager.hpp"
#include "SuperBlock.hpp"
#include "BufferManager.hpp"
#include "DirectoryEntry.hpp"
#include "DiskDriver.hpp"

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

        _curDirInodeNo = 0;

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

    Error FileManager::ls()
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to list all files in current directory
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Read all directory entries in current directory inode with Inode::read()
        // (3) Print all directory entries' file name

        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);

        // TODO: Properly resolve all data blocks
        int blkno = curDirInode.i_addr[0];
        Buf *buf = BufferManager::getInstance()->bread(blkno);
        DirectoryEntry *pDirEntry = (DirectoryEntry *)buf->b_addr;
        BufferManager::getInstance()->brelse(buf);
        std::string result;
        for (size_t i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry); ++i)
        {
            std::string name = pDirEntry[i]._name;
            result.append(name);
            result += " \n"[i == curDirInode.i_size / sizeof(DirectoryEntry) - 1];
        }

        Log::out(result, rang::fg::blue);

        return ec;
    }

    Error FileManager::cd(const std::string &dirName)
    {
        const Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);
        Buf *bp = BufferManager::getInstance()->bread(curDirInode.i_addr[0]);
        DirectoryEntry *pDirEntry = (DirectoryEntry *)bp->b_addr;
        BufferManager::getInstance()->brelse(bp);
        for (int i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry); ++i)
        {
            if (strcmp(pDirEntry->_name, dirName.c_str()) == 0)
            {
                const Inode &dirInode = InodeTable::getInstance()->iget(pDirEntry->_ino);
                if (dirInode.i_mode != Inode::IFDIR)
                    return Error::UFS_ERR_NOT_A_DIR;

                _curDirInodeNo = pDirEntry->_ino;
                return Error::UFS_NOERR;
            }

            pDirEntry++;
        }

        return Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
    }

    Error FileManager::mkdir(const std::string &dirName, bool isRoot)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to create a new directory
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Allocate a new inode with InodeTable::ialloc()
        // (3) Write directory entry to data block pointed by i_addr

        Inode newDirInode;
        newDirInode.i_mode = Inode::IFDIR;
        newDirInode.i_nlink = 1;
        newDirInode.i_count = 1;
        newDirInode.i_flag |= (Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC);
        // directory is always initted with "." and ".."
        newDirInode.i_size = 2 * sizeof(DirectoryEntry);

        // Allocate a new inode
        SuperBlock &sb = SuperBlockManager::getInstance()->superBlock();
        int newDirInodeNo = sb.ialloc();
        newDirInode.i_number = newDirInodeNo;
        newDirInode.i_addr[0] = sb.balloc();
        InodeTable::getInstance()->iupdate(newDirInode.i_number, newDirInode);

        // Initialize directory entry
        InodeTable::getInstance()->addDirectoryEntry(newDirInodeNo, ".", newDirInodeNo);

        if (!isRoot)
            InodeTable::getInstance()->addDirectoryEntry(newDirInodeNo, "..", _curDirInodeNo);

        else
            InodeTable::getInstance()->addDirectoryEntry(newDirInodeNo, "..", newDirInodeNo);

        // update current directory inode
        if (!isRoot)
        {
            InodeTable::getInstance()->addDirectoryEntry(_curDirInodeNo, dirName, newDirInodeNo);
        }

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
        tmpSB.s_fmod = true;
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

        SuperBlockManager::getInstance()->setSuperBlock(tmpSB);

        mkdir("root", true);
        mkdir("home");
        mkdir("usr");
        mkdir("bin");
        mkdir("etc");
        mkdir("dev");

        // Flush all new inodes back to disk
        InodeTable::getInstance()->flushAllDirtyInodeCache();

        // Flush SuperBlock to disk
        SuperBlockManager::getInstance()->flushSuperBlockCache();

        // Flush all dirty buffers back to disk
        BufferManager::getInstance()->flush();

        return ec;
    }
}