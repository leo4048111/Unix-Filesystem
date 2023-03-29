#pragma once

#include "Log.hpp"
#include "Defines.hpp"
#include "SuperBlock.hpp"
#include "Inode.hpp"

#include <vector>

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

        void addDirectoryEntry(int inodeId, const std::string &name, const int ino);
        DirectoryEntry& dirEntryAt(Inode& inode, int idx, bool willModifyEntry = false); // find the idx-th directory entry in the directory
        void removeDirEntryAt(Inode& inode, int idx); // remove the idx-th directory entry in the directory
        Error fwrite(Inode& inode, std::vector<BYTE> &buffer); // write len bytes from buf to the file
        Error fread(Inode& inode, std::vector<BYTE> &buffer); // read len bytes from the file to buf
        Error freeInode(Inode& inode); // free an inode
        Error freeThisInodeAndAllSubInodes(Inode& inode); // free sub inodes in a directory

    private:
        unsigned int _fsStat { FS_UNINITIALIZED };
    };
}