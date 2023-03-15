#include "FileManager.hpp"

#include "InodeTable.hpp"

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

        return Error::UFS_NOERR;
    }

    Error FileManager::unmount()
    {
        return Error::UFS_NOERR;
    }
}