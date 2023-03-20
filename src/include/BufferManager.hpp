#pragma once

#include "Buf.hpp"

namespace ufs
{
    class BufferManager
    {
        SINGLETON(BufferManager)

    private:
        static const size_t NBUF = 30; // number of buffers

    public:
        Error mount();

        Error unmount();

    public:
        void initialize(); // initialize buffer manager
        
        void flush();      // flush all dirty blocks to disk

        void notAvail(Buf *bp); // remove bp from free list

        void brelse(Buf *bp); // release a buffer, put it into free list

        void bdwrite(Buf *bp); // delay write, add B_DELWRI and B_DONE to a buffer, than release it
        void bwrite(Buf *bp); // clear B_DELWRI B_READ B_DONE B_ERROR flags, write buffer to disk

        Buf *bread(int blkno); // read DiskBlock blkno from disk, return a buffer

        Buf *getBlk(int blkno); // alloc a buffer to read/write DiskBlock blkno

    private:
        Buf _bFreeList;              // head of free list
        Buf _bufs[NBUF];             // buffer control blocks
        DiskBlock _diskBlocks[NBUF]; // disk blocks

        void debug_print(const char* msg);
    };
}