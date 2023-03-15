#include "FileManager.hpp"

namespace ufs
{
    Error FileManager::mount()
    {
        // Steps to mount the disk
        // (1) initialize memory InodeCache with InodeCache::clearCache()
        // (2) Open the disk image file with DiskDriver::mount()
        // (3) Load SuperBlock to SuperBlock cache

        
    }
    Error FileManager::unmount()
    {

    }
}