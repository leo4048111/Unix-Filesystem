#include "InodeTable.hpp"
#include "FileSystem.hpp"
#include "BufferManager.hpp"

#include <string.h>

namespace ufs
{
    std::unique_ptr<InodeTable> InodeTable::_inst;

    InodeTable::InodeTable()
    {
        memset(this, 0, sizeof(InodeTable));
    }

    InodeTable::~InodeTable()
    {
    }

    void InodeTable::iupdate(int inodeId, Inode& inode)
    {
        _inodes[inodeId] = inode;
    }

    void InodeTable::iread(int inodeId, Inode& inode)
    {
        DiskInode dinode;
        int blkno = 1 + inodeId / (DISK_BLOCK_SIZE / DISKINODE_SIZE);
        Buf *bp = BufferManager::getInstance()->bread(blkno);
        uintptr_t destAddr = (uintptr_t)bp->b_addr;
        destAddr = destAddr + (inodeId % (DISK_BLOCK_SIZE / DISKINODE_SIZE)) * DISKINODE_SIZE;
        memcpy_s(&dinode, DISKINODE_SIZE, (void*)destAddr, DISKINODE_SIZE);
        BufferManager::getInstance()->brelse(bp);
        inode = Inode(dinode);
        inode.i_count = 1;
    }

    Inode& InodeTable::iget(int inodeId)
    {
        if(!isInodeCacheLoaded(inodeId))
        {
            // load inodeId from disk
            iread(inodeId, _inodes[inodeId]); 
        }

        return _inodes[inodeId];
    }

    bool InodeTable::isInodeCacheLoaded(int inodeId)
    {
        return _inodes[inodeId].i_count != 0;
    }

    void InodeTable::clear()
    {
        // clear all inode cached in memory
        memset(_inodes, 0, sizeof(_inodes));
    }

    void InodeTable::flushAllDirtyInodeCache()
    {
        // Flush everything into disk(for debugging purposes)
        // for (size_t i = 0; i < NINODE; i++)
        // {
        //     if ((_inodes[i].i_count != 0) &&
        //         (_inodes[i].i_flag & (Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC)))
        //     {
        //         FileSystem::getInstance()->writeInodeCacheBackToDisk(_inodes[i]);
        //     }
        // }

        DiskBlock* pPartition = (DiskBlock*)this;

        for(size_t i = 1; i <= 3; i++)
        {
            Buf* bp = BufferManager::getInstance()->getBlk(i);
            memcpy_s(bp->b_addr, DISK_BLOCK_SIZE, pPartition, DISK_BLOCK_SIZE);
            pPartition++;
            BufferManager::getInstance()->bdwrite(bp);
        }
    }

    void InodeTable::loadInodeCacheFromDisk()
    {
        Buf *bp;
        // read data from 1# ~ 3# disk block
        for (size_t i = 1; i <= 3; i++)
        {
            bp = BufferManager::getInstance()->bread(i);
            memcpy_s(this + (i - 1) * DISK_BLOCK_SIZE, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
            BufferManager::getInstance()->brelse(bp);
        }
    }
}