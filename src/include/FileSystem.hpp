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

        DirectoryEntry& dirEntryAt(Inode& inode, int idx); // find the idx-th directory entry in the directory
        Error fwrite(Inode& inode, const std::string& buffer); // write len bytes from buf to the file
        Error fread(Inode& inode, std::string& buffer); // read len bytes from the file to buf

    private:
        unsigned int _fsStat { FS_UNINITIALIZED };
    };
}