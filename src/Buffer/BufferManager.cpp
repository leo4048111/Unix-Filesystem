#include "BufferManager.hpp"
#include "DiskDriver.hpp"

namespace ufs
{
    BufferManager::BufferManager()
    {
    }

    BufferManager::~BufferManager()
    {
    }

    Error BufferManager::mount()
    {
        initialize();
        return DiskDriver::getInstance()->mount();
    }

    Error BufferManager::unmount()
    {
        flush();
        return DiskDriver::getInstance()->unmount();
    }

    void BufferManager::flush()
    {
        for (Buf *bp = _bFreeList.av_forw; bp != &_bFreeList; bp = bp->av_forw)
            if (bp->b_flags & Buf::BufFlag::B_DELWRI)
                bwrite(bp); // find every buffer which has B_DELWRI flag, write it to disk
    }

    void BufferManager::initialize()
    {
        _bFreeList.b_forw = _bFreeList.b_back = &_bFreeList;   // initialize NODEV list
        _bFreeList.av_forw = _bFreeList.av_back = &_bFreeList; // initialize free list

        for (size_t i = 0; i < NBUF; i++)
        {
            Buf *bp = &_bufs[i];
            bp->b_blkno = -1;
            bp->b_dev = -1;
            bp->b_addr = &_diskBlocks[i];
            bp->b_flags = Buf::BufFlag::B_BUSY;

            brelse(bp); // put bp into free list
        }
    }

    void BufferManager::bwrite(Buf* bp)
    {
        unsigned int flags;

        flags = bp->b_flags;
        bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
        bp->b_wcount = DISK_BLOCK_SIZE;

        if(flags & Buf::BufFlag::B_DELWRI)
        {
            // write buffer to disk
            DiskDriver::getInstance()->writeBlk(bp->b_blkno, *bp->b_addr);
            bp->b_flags |= Buf::B_DONE;
        }
    }

    void BufferManager::brelse(Buf *bp)
    {
        // clear B_WANTED, B_BUSY, B_ASYNC flags
        bp->b_flags &= ~(
            Buf::BufFlag::B_WANTED |
            Buf::BufFlag::B_BUSY |
            Buf::BufFlag::B_ASYNC);

        // insert bp into free list tail
        _bFreeList.av_back->av_forw = bp;
        bp->av_back = _bFreeList.av_back;
        bp->av_forw = &_bFreeList;
        _bFreeList.av_back = bp;
    }
}