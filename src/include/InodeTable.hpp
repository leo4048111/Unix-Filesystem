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

        void iupdate(int inodeId, Inode& inode);

    private:
        Inode _inodes[NINODE];
        char padding[4688];
    };
}