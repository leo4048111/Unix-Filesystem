#include "FileManager.hpp"
#include "FileSystem.hpp"
#include "InodeTable.hpp"
#include "SuperBlockManager.hpp"

#include "Log.hpp"

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

        InodeTable::getInstance()->clear();

        Error ec = FileSystem::getInstance()->mount();

        if(ec == Error::UFS_NOERR)
            _isMounted = true;

        return ec;
    }

    Error FileManager::unmount()
    {
        Error ec = Error::UFS_NOERR;
        if(!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to unmount the disk
        // (1) Flush all dirty InodeCache to disk with InodeCache::flushAllDirtyInodeCache()
        // (2) Flush SuperBlock cache to disk with SuperBlock::flushSuperBlock()
        // (3) Close the disk image file with DiskDriver::unmount()
        InodeTable::getInstance()->flushAllDirtyInodeCache();

        if(SuperBlockManager::getInstance()->isDirty())
        {
            SuperBlockManager::getInstance()->flushSuperBlockCache();
        }

        ec = FileSystem::getInstance()->unmount();

        if(ec == Error::UFS_NOERR)
            _isMounted = false;

        return ec;
    }
}