#include "FileManager.hpp"
#include "FileSystem.hpp"
#include "InodeTable.hpp"
#include "SuperBlockManager.hpp"
#include "SuperBlock.hpp"
#include "BufferManager.hpp"

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

        if(isMounted())
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
        Error ec = Error::UFS_NOERR;

        if(!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

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
        tmpSB.s_nfree = 50;
        for (tmpSB.s_nfree = 0; tmpSB.s_nfree < 50; tmpSB.s_nfree++)
            tmpSB.s_free[tmpSB.s_nfree] = 49 - tmpSB.s_nfree;

        // init inode table
        for (tmpSB.s_ninode = 0; tmpSB.s_ninode < 50; tmpSB.s_ninode++)
            tmpSB.s_inode[tmpSB.s_ninode] = 49 - tmpSB.s_ninode;

        // init root directory
        int superBlockDiskBlkno = tmpSB.balloc(); // alloc 0# diskblock to store superblock
        int inodepoolDiskBlkno[3] = {
            tmpSB.balloc(),
            tmpSB.balloc(),
            tmpSB.balloc()}; // aloc 1# ~ 3# diskblock to store inode pool

        // TODO basic file tree struct

        // Flush SuperBlock to disk
        SuperBlockManager::getInstance()->setSuperBlock(tmpSB);
        SuperBlockManager::getInstance()->flushSuperBlockCache();

        return ec;
    }
}