#include "InodeTable.hpp"
#include "FileSystem.hpp"
#include "BufferManager.hpp"

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

    void InodeTable::iupdate(int inodeId, Inode& inode)
    {
        _inodes[inodeId] = inode;
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

    void InodeTable::loadInodeCacheFromDisk()
    {
        Buf *bp;
        // read data from 1# disk block
        for (size_t i = 1; i <= 3; i++)
        {
            bp = BufferManager::getInstance()->bread(i);
            memcpy_s(this + (i - 1) * DISK_BLOCK_SIZE, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
            BufferManager::getInstance()->brelse(bp);
        }
    }
}