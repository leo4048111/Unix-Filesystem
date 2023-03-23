#include "Inode.hpp"
#include "BufferManager.hpp"

#include <string.h>

namespace ufs
{
    Inode::Inode(){
        memset(this, 0, sizeof(Inode));
    };

    Inode::Inode(const Inode& inode)
    {
        memcpy_s(this, sizeof(Inode), &inode, sizeof(Inode));
    }

    Inode& Inode::operator= (const Inode& inode)
    {
        memcpy_s(this, sizeof(Inode), &inode, sizeof(Inode));
        return *this;
    }

    DirectoryEntry& Inode::dirEntryAt(int idx)
    {
        size_t totalSize = this->i_size;

        size_t numDirectEntries = totalSize / sizeof(DirectoryEntry);

        size_t blkno = bmap(idx / (DISK_BLOCK_SIZE / sizeof(DirectoryEntry)));
        Buf *bp = BufferManager::getInstance()->bread(blkno);
        DirectoryEntry* dirEntry = (DirectoryEntry*)bp->b_addr;

        dirEntry += (idx % (DISK_BLOCK_SIZE / sizeof(DirectoryEntry)));

        BufferManager::getInstance()->brelse(bp);

        return *dirEntry;
    }

    Inode::Inode(DiskInode &diskInode)
    {
        this->i_mode = diskInode.d_mode;
        this->i_nlink = diskInode.d_nlink;
        this->i_uid = diskInode.d_uid;
        this->i_gid = diskInode.d_gid;
        this->i_size = diskInode.d_size;
        memcpy(this->i_addr, diskInode.d_addr, sizeof(diskInode.d_addr));
        this->i_flag = 0;
        this->i_count = 0;
        this->i_dev = 0;
        this->i_lastr = -1;
    };

    void Inode::fwrite(const std::string& buffer)
    {
        size_t totalSize = this->i_size;
        int blkno = totalSize / DISK_BLOCK_SIZE;
        Buf* bp = BufferManager::getInstance()->bread(blkno);

        size_t offset = totalSize % DISK_BLOCK_SIZE;

        // TODO: if current available space in the block is not enough, we need to allocate a new block

        uintptr_t destAddr = (uintptr_t)bp->b_addr;
        destAddr += offset;
        memcpy_s((void*)destAddr, buffer.size(), buffer.c_str(), buffer.size());

        BufferManager::getInstance()->brelse(bp);
    }

    int Inode::bmap(int lbn)
    {
        return i_addr[lbn];
    };
}