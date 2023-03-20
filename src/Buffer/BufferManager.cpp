#include "BufferManager.hpp"
#include "DiskDriver.hpp"

namespace ufs
{
    std::unique_ptr<BufferManager> BufferManager::_inst;

    BufferManager::BufferManager()
    {
        memset(this, 0, sizeof(BufferManager));
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

        // for (Buf *bp = _bFreeList.av_back; bp != &_bFreeList; bp = bp->av_back)
        //     if (bp->b_flags & Buf::BufFlag::B_DELWRI)
        //         bwrite(bp); // find every buffer which has B_DELWRI flag, write it to disk
    }

    // 0x27d2508

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

        debug_print("initialize");
    }

    void BufferManager::bwrite(Buf *bp)
    {
        unsigned int flags;

        flags = bp->b_flags;
        bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
        bp->b_wcount = DISK_BLOCK_SIZE;

        // bwrite should only be called when a buffer with DELWRI is to be written to disk
        if (flags & Buf::BufFlag::B_DELWRI)
        {
            // write buffer to disk
            DiskDriver::getInstance()->writeBlk(bp->b_blkno, *bp->b_addr);
            bp->b_flags |= Buf::B_DONE;
        }
    }

    void BufferManager::bdwrite(Buf *bp)
    {
        bp->b_flags |= (Buf::BufFlag::B_DELWRI | Buf::BufFlag::B_DONE);
        brelse(bp);
    }

    Buf *BufferManager::getBlk(int blkno)
    {
        Buf *bp;

        for (bp = _bFreeList.av_forw; bp != &_bFreeList; bp = bp->av_forw)
        {
            if (bp->b_blkno != blkno)
                continue;

            // found a buffer to reuse, remove it from free list
            notAvail(bp);
            return bp;
        }

        bp = _bFreeList.av_forw; // get a buffer from the head of free list
        notAvail(bp);

        if (bp->b_flags & Buf::BufFlag::B_DELWRI)
            bwrite(bp); // if the buffer is dirty, write it to disk

        // clear every flag except B_BUSY
        bp->b_flags = Buf::BufFlag::B_BUSY;
        bp->b_blkno = blkno;

        // if buf is not previously in device buffer list, insert it into device buffer list
        if (bp->b_dev != DiskDriver::getInstance()->devno())
        {
            bp->b_back = &_bFreeList;
            bp->b_forw = _bFreeList.b_forw;
            _bFreeList.b_forw->b_back = bp;
            _bFreeList.b_forw = bp;
            bp->b_dev = DiskDriver::getInstance()->devno();
        }

        return bp;
    }

    void BufferManager::notAvail(Buf *bp)
    {
        // remove bp from free list
        bp->av_forw->av_back = bp->av_back;
        bp->av_back->av_forw = bp->av_forw;

        bp->b_flags |= Buf::BufFlag::B_BUSY;

        debug_print("notAvail");
    }

    Buf *BufferManager::bread(int blkno)
    {
        Buf *bp;
        bp = getBlk(blkno);

        // if blkno disk block is already cached, reuse it
        if (bp->b_flags & Buf::BufFlag::B_DONE)
            return bp;

        // if blkno disk block is not cached, read it from disk
        bp->b_flags |= Buf::BufFlag::B_READ;
        bp->b_wcount = DISK_BLOCK_SIZE;
        DiskDriver::getInstance()->readBlk(blkno, *bp->b_addr);
        bp->b_wcount |= Buf::BufFlag::B_DONE;

        debug_print("bread");

        return bp;
    }

    void BufferManager::brelse(Buf *bp)
    {
        // clear B_WANTED, B_BUSY, B_ASYNC flags
        bp->b_flags &= ~(
            Buf::BufFlag::B_WANTED |
            Buf::BufFlag::B_BUSY |
            Buf::BufFlag::B_ASYNC);

        // insert bp into free list tail

        if (_bFreeList.av_back == &_bFreeList)
        {
            _bFreeList.av_back = _bFreeList.av_forw = bp;
            bp->av_forw = &_bFreeList;
            bp->av_back = &_bFreeList;
        }
        else
        {
            _bFreeList.av_back->av_forw = bp;
            bp->av_back = _bFreeList.av_back;
            bp->av_forw = &_bFreeList;
            _bFreeList.av_back = bp;
        }

        debug_print("brelse");
    }

    void BufferManager::debug_print(const char* msg)
    {
        printf("%s\n", msg);
        printf("av_forw: ");
        for(Buf* bp = _bFreeList.av_forw; bp != &_bFreeList; bp = bp->av_forw)
            printf("%d ", bp->b_blkno);
        printf("\n");

        printf("av_back: ");
        for(Buf* bp = _bFreeList.av_back; bp != &_bFreeList; bp = bp->av_back)
            printf("%d ", bp->b_blkno);
        printf("\n");
    }
}