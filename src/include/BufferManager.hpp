#pragma once

#include <memory>

#include "Buf.hpp"

namespace ufs
{
    class BufferManager
    {
    private:
        static const size_t NBUF = 30; // number of buffers

    private:
        BufferManager();
        BufferManager(const BufferManager &) = delete;
        BufferManager &operator=(const BufferManager &) = delete;

        static std::unique_ptr<BufferManager> _inst;

    public:
        static BufferManager *getInstance()
        {
            if (_inst.get() == nullptr)
                _inst.reset(new BufferManager);

            return _inst.get();
        }

    public:
        ~BufferManager();

        Error mount();

        Error unmount();

    private:
        void initialize(); // initialize buffer manager
        void flush();      // flush all dirty blocks to disk

        void brelse(Buf *bp); // release a buffer, put it into free list

        void bwrite(Buf* bp); // clear B_DELWRI B_READ B_DONE B_ERROR flags, write buffer to disk
   
    private:
        Buf _bFreeList;              // head of free list
        Buf _bufs[NBUF];             // buffer control blocks
        DiskBlock _diskBlocks[NBUF]; // disk blocks
    };
}