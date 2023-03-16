#include "InodeTable.hpp"
#include "FileSystem.hpp"

#include <string.h>

namespace ufs
{
    std::unique_ptr<InodeTable> InodeTable::_inst;

    InodeTable::InodeTable()
    {
    }

    InodeTable::~InodeTable()
    {
    }

    void InodeTable::clear()
    {
        // clear all inode cached in memory
        memset(_inodes, 0, sizeof(_inodes));
    }

    void InodeTable::flushAllDirtyInodeCache()
    {
        for (size_t i = 0; i < NINODE; i++)
        {
            if ((_inodes[i].i_count != 0) &&
                (_inodes[i].i_flag & (Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC)))
            {
                FileSystem::getInstance()->writeInodeCacheBackToDisk(_inodes[i]);
            }
        }
    }
}