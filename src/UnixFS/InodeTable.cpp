#include "InodeTable.hpp"
#include "FileSystem.hpp"
#include "BufferManager.hpp"
#include "DirectoryEntry.hpp"
#include "SuperBlockManager.hpp"

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

    void InodeTable::iupdate(int inodeId, Inode &inode)
    {
        UFS_DEBUG_INFO(Log::format("iupdate: inode %d", inodeId));
        _inodes[inodeId] = inode;
        _inodes[inodeId].i_flag |= Inode::INodeFlag::IUPD;
    }

    void InodeTable::iexpand(int inodeId, const int size)
    {
        UFS_DEBUG_INFO(Log::format("iexpand: inode %d, size %d", inodeId, size));
        SuperBlock &sb = SuperBlockManager::getInstance()->superBlock();
        Inode &inode = iget(inodeId);
        inode.i_flag |= Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC;
        int curDiskBlockCount = inode.i_size / DISK_BLOCK_SIZE;
        inode.i_size += size;
        int newDiskBlockCount = inode.i_size / DISK_BLOCK_SIZE;
        if (newDiskBlockCount > curDiskBlockCount)
        {
            // need to allocate new disk blocks
            for (int i = curDiskBlockCount; i < newDiskBlockCount; i++)
            {
                int newBlock = SuperBlockManager::getInstance()->superBlock().balloc();
                if (i + 1 < Inode::SMALL_FILE_BLOCK)
                    inode.i_addr[i + 1] = newBlock;
                else if (i + 1 < Inode::LARGE_FILE_BLOCK)
                {
                    int lastIndirectBlock = (i - Inode::SMALL_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK + Inode::SMALL_FILE_BLOCK;
                    int indirectBlock = (i + 1 - Inode::SMALL_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK + Inode::SMALL_FILE_BLOCK;
                    if (i < Inode::SMALL_FILE_BLOCK) // alloc i_addr[6]
                        inode.i_addr[indirectBlock] = sb.balloc();
                    else if (lastIndirectBlock < indirectBlock) // alloc i_addr[7]
                        inode.i_addr[indirectBlock] = sb.balloc();
                    Buf *bp = BufferManager::getInstance()->bread(inode.i_addr[indirectBlock]);
                    int *addr = (int *)bp->b_addr;
                    addr[(i + 1 - Inode::SMALL_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK] = newBlock;
                    BufferManager::getInstance()->bdwrite(bp);
                }
                else if (i + 1 < Inode::HUGE_FILE_BLOCK)
                {
                    int indirectDoubleBlknoFirst = (i + 1 - Inode::LARGE_FILE_BLOCK) / (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK) + Inode::SMALL_FILE_BLOCK + 2;
                    if(i < Inode::LARGE_FILE_BLOCK)
                        inode.i_addr[indirectDoubleBlknoFirst] = sb.balloc();

                    Buf* bp = BufferManager::getInstance()->bread(inode.i_addr[indirectDoubleBlknoFirst]);
                    int* addr = (int*)bp->b_addr;
                    
                    int curIndirectDoubleBlknoSecond = ((i + 1 - Inode::LARGE_FILE_BLOCK) % (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK)) / Inode::ADDRESS_PER_INDEX_BLOCK;
                    int lastIndirectDoubleBlknoSecond = curIndirectDoubleBlknoSecond;
                    if(i >= Inode::LARGE_FILE_BLOCK)
                        lastIndirectDoubleBlknoSecond = ((i - Inode::LARGE_FILE_BLOCK) % (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK)) / Inode::ADDRESS_PER_INDEX_BLOCK;
                    if(curIndirectDoubleBlknoSecond > lastIndirectDoubleBlknoSecond)
                        addr[curIndirectDoubleBlknoSecond] = sb.balloc();
                    BufferManager::getInstance()->bdwrite(bp);
                }
            }
        }
        else if(newDiskBlockCount < curDiskBlockCount)
        {
            for (int i = curDiskBlockCount; i > newDiskBlockCount; i--)
            {
                int blkno = inode.bmap(i);
                sb.bfree(blkno);
                if((curDiskBlockCount - Inode::SMALL_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK == 0)
                {
                    int indirectBlock = (i - Inode::SMALL_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK + Inode::SMALL_FILE_BLOCK;
                    sb.bfree(inode.i_addr[indirectBlock]);
                }
                else if((curDiskBlockCount - Inode::LARGE_FILE_BLOCK) % (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK) == 0)
                {
                    int indirectDoubleBlknoFirst = (i - Inode::LARGE_FILE_BLOCK) / (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK) + Inode::SMALL_FILE_BLOCK + 2;
                    int indirectDoubleBlknoSecond = (i - Inode::LARGE_FILE_BLOCK) % (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK;
                    Buf* bp = BufferManager::getInstance()->bread(inode.i_addr[indirectDoubleBlknoFirst]);
                    int* addr = (int*)bp->b_addr;
                    sb.bfree(addr[indirectDoubleBlknoSecond]);
                    BufferManager::getInstance()->brelse(bp);
                }
            }
        }
    }

    Inode &InodeTable::iread(int inodeId)
    {
        UFS_DEBUG_INFO(Log::format("Preparing to iread inode %d from disk", inodeId));

        DiskInode dinode;
        int blkno = 1 + inodeId / (DISK_BLOCK_SIZE / DISKINODE_SIZE);
        Buf *bp = BufferManager::getInstance()->bread(blkno);
        DiskInode *destAddr = (DiskInode *)bp->b_addr;
        destAddr += (inodeId % (DISK_BLOCK_SIZE / DISKINODE_SIZE));
        memcpy_s(&dinode, DISKINODE_SIZE, (void *)destAddr, DISKINODE_SIZE);
        BufferManager::getInstance()->brelse(bp);

        _inodes[inodeId] = Inode(dinode);
        _inodes[inodeId].i_count = 1;
        _inodes[inodeId].i_number = inodeId;

        UFS_DEBUG_INFO(Log::format("End ireading inode %d from disk", inodeId));

        return _inodes[inodeId];
    }

    Inode &InodeTable::iget(int inodeId)
    {
        UFS_DEBUG_INFO(Log::format("iget inode %d", inodeId));
        if (!isInodeCacheLoaded(inodeId))
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

    template <>
    void InodeTable::iwrite<std::vector<BYTE>>(int inodeId, std::vector<BYTE> &data)
    {
        Inode& inode = iget(inodeId);
        size_t curSize = inode.i_size;
        iexpand(inodeId, data.size());
        size_t newSize = inode.i_size;
        size_t sizeToWrite = newSize - curSize;
        
        int curBlockCnt = curSize / DISK_BLOCK_SIZE;
        int newBlockCnt = newSize / DISK_BLOCK_SIZE;

        size_t firstBlockEmptySize = DISK_BLOCK_SIZE - curSize % DISK_BLOCK_SIZE;
        size_t firstBlockWriteSize = sizeToWrite > firstBlockEmptySize ? firstBlockEmptySize : sizeToWrite;

        uintptr_t pData = (uintptr_t)data.data();

        // write to first block
        Buf* bp = BufferManager::getInstance()->bread(inode.bmap(curBlockCnt));
        uintptr_t destAddr = (uintptr_t)bp->b_addr + curSize % DISK_BLOCK_SIZE;
        memcpy_s((void*)destAddr, firstBlockWriteSize, (void*)pData, firstBlockWriteSize);
        BufferManager::getInstance()->bdwrite(bp);
        sizeToWrite -= firstBlockWriteSize;
        pData += firstBlockWriteSize;

        for(int i = curBlockCnt + 1; i <= newBlockCnt; i++)
        {
            int blkno = inode.bmap(i);
            bp = BufferManager::getInstance()->bread(blkno);
            size_t tmpSize = sizeToWrite > DISK_BLOCK_SIZE ? DISK_BLOCK_SIZE : sizeToWrite;
            memcpy_s(bp->b_addr, tmpSize, (void*)pData, tmpSize);
            pData += tmpSize;
            sizeToWrite -= tmpSize;
            BufferManager::getInstance()->bdwrite(bp);
        }

        iupdate(inodeId, inode);
    }
}