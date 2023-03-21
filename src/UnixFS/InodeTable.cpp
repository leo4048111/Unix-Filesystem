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
        UFS_DEBUG_INFO(Log::format("iupdate: inode %d", inodeId));
        _inodes[inodeId] = inode;
        _inodes[inodeId].i_flag |= Inode::INodeFlag::IUPD;
    }

    Inode& InodeTable::iread(int inodeId)
    {
        UFS_DEBUG_INFO(Log::format("Preparing to iread inode %d from disk", inodeId));

        DiskInode dinode;
        int blkno = 1 + inodeId / (DISK_BLOCK_SIZE / DISKINODE_SIZE);
        Buf *bp = BufferManager::getInstance()->bread(blkno);
        DiskInode* destAddr = (DiskInode*)bp->b_addr;
        destAddr += (inodeId % (DISK_BLOCK_SIZE / DISKINODE_SIZE));
        memcpy_s(&dinode, DISKINODE_SIZE, (void*)destAddr, DISKINODE_SIZE);
        BufferManager::getInstance()->brelse(bp);

        _inodes[inodeId] = Inode(dinode);
        _inodes[inodeId].i_count = 1;
        _inodes[inodeId].i_number = inodeId;

        UFS_DEBUG_INFO(Log::format("End ireading inode %d from disk", inodeId));
    
        return _inodes[inodeId];
    }

    Inode& InodeTable::iget(int inodeId)
    {
        UFS_DEBUG_INFO(Log::format("iget inode %d", inodeId));
        if(!isInodeCacheLoaded(inodeId))
        {
            // load inodeId from disk
            iupdate(inodeId, iread(inodeId)); 
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
        UFS_DEBUG_INFO("Clearing all inode cache...");
        memset(this, 0, sizeof(InodeTable));
    }

    void InodeTable::flushAllDirtyInodeCache()
    {
        UFS_DEBUG_INFO("Flushing all dirty inode cache to disk...");
        for (size_t i = 0; i < NINODE; i++)
        {
            if ((_inodes[i].i_count != 0) &&
                (_inodes[i].i_flag & (Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC)))
            {
                FileSystem::getInstance()->writeInodeCacheBackToDisk(_inodes[i]);
            }
        }
        UFS_DEBUG_INFO("Flush complete.");
        // DiskBlock* pPartition = (DiskBlock*)this;

        // for(size_t i = 1; i <= 3; i++)
        // {
        //     Buf* bp = BufferManager::getInstance()->getBlk(i);
        //     memcpy_s(bp->b_addr, DISK_BLOCK_SIZE, pPartition, DISK_BLOCK_SIZE);
        //     pPartition++;
        //     BufferManager::getInstance()->bdwrite(bp);
        // }
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