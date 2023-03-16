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

    private:
        Inode _inodes[NINODE];
    };
}