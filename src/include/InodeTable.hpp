#pragma once

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

        void iread(int inodeId, Inode& inode);

    private:
        Inode _inodes[NINODE];
        char padding[4688];
    };
}