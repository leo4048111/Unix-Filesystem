#pragma once

#include "Log.hpp"
#include "Defines.hpp"
#include "Inode.hpp"

namespace ufs
{
    class InodeTable
    {
        SINGLETON(InodeTable)

    public:
        static const int NINODE = 100;

    public:
        void clear();

        void flushAllDirtyInodeCache();

        void loadInodeCacheFromDisk();

        bool isInodeCacheLoaded(int inodeId);

        void iupdate(int inodeId, Inode& inode);

        Inode& iget(int inodeId);

        Inode& iread(int inodeId);

        Error addDirectoryEntry(int inodeId, const std::string& name, const int ino);

    private:
        Inode _inodes[NINODE];
        char padding[4688];
    };
}