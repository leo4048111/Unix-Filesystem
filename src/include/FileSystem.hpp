#pragma once

#include "Log.hpp"
#include "Defines.hpp"
#include "SuperBlock.hpp"
#include "Inode.hpp"

namespace ufs
{
    class FileSystem
    {
        SINGLETON(FileSystem)

    private:
        enum FS_STATUS
        {
            FS_UNINITIALIZED, // uninitialized
            FS_NOFORM,        // initialized but not formatted
            FS_READY          // initialized and formatted
        };

    public:
        Error mount();
        Error unmount();

        void loadSuperBlock(SuperBlock& superblock);

        void writeInodeCacheBackToDisk(Inode& inode);
        
    private:
        unsigned int _fsStat { FS_UNINITIALIZED };
    };
}