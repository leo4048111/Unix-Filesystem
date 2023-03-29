#pragma once

#include "Log.hpp"
#include "Defines.hpp"
#include "Inode.hpp"
#include "BufferManager.hpp"

#include <vector>

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

        void iupdate(int inodeId, Inode &inode);

        Inode &iget(int inodeId);

        Inode &iread(int inodeId);

        template <typename T>
        void iwrite(int inodeId, T &data);

        void iexpand(int inodeId, const int size); // note that size can be negative

        Error addDirectoryEntry(int inodeId, const std::string &name, const int ino);

    private:
        Inode _inodes[NINODE];
        char padding[4688];
    };

    template<>
    void InodeTable::iwrite<std::vector<BYTE>>(int inodeId, std::vector<BYTE> &data);

    template <typename T>
    void InodeTable::iwrite(int inodeId, T &data)
    {
        Inode& inode = iget(inodeId);
        size_t curSize = inode.i_size;
        iexpand(inodeId, sizeof(T));
        size_t newSize = inode.i_size;
        size_t sizeToWrite = newSize - curSize;
        
        int curBlockCnt = curSize / DISK_BLOCK_SIZE;
        int newBlockCnt = newSize / DISK_BLOCK_SIZE;

        size_t firstBlockEmptySize = DISK_BLOCK_SIZE - curSize % DISK_BLOCK_SIZE;
        size_t firstBlockWriteSize = sizeToWrite > firstBlockEmptySize ? firstBlockEmptySize : sizeToWrite;

        uintptr_t pData = (uintptr_t)&data;

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