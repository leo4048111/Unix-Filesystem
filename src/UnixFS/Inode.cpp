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

    int Inode::bmap(int lbn)
    {
        // i_addr[0] ~ i_addr[5] are direct indices
        if(lbn < Inode::SMALL_FILE_BLOCK)
            return i_addr[lbn];
        else if(lbn < Inode::LARGE_FILE_BLOCK){
            // i_addr[6] ~ i_addr[7] are single indirect indices
            int indirectSingleBlkno = (lbn - Inode::SMALL_FILE_BLOCK) / Inode::ADDRESS_PER_INDEX_BLOCK;
            Buf* bp = BufferManager::getInstance()->bread(i_addr[indirectSingleBlkno]);
            int lbns[Inode::ADDRESS_PER_INDEX_BLOCK];
            memcpy_s(lbns, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
            BufferManager::getInstance()->brelse(bp);
            int indirectLbn = (lbn - Inode::SMALL_FILE_BLOCK) % Inode::ADDRESS_PER_INDEX_BLOCK;
            return lbns[indirectLbn];
        }
        else if(lbn < Inode::HUGE_FILE_BLOCK){
            // i_addr[8] ~ i_addr[9] are double indirect indices
            int indirectDoubleBlknoFirst = (lbn - Inode::LARGE_FILE_BLOCK) / (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK);
            Buf* bp = BufferManager::getInstance()->bread(i_addr[indirectDoubleBlknoFirst]);
            int lbns[Inode::ADDRESS_PER_INDEX_BLOCK];
            memcpy_s(lbns, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
            BufferManager::getInstance()->brelse(bp);

            int indirectDoubleBlknoSecond = ((lbn - Inode::LARGE_FILE_BLOCK) % (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK)) / Inode::ADDRESS_PER_INDEX_BLOCK;
            bp = BufferManager::getInstance()->bread(lbns[indirectDoubleBlknoSecond]);
            memcpy_s(lbns, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
            BufferManager::getInstance()->brelse(bp);

            int indirectLbn = ((lbn - Inode::LARGE_FILE_BLOCK) % (Inode::ADDRESS_PER_INDEX_BLOCK * Inode::ADDRESS_PER_INDEX_BLOCK)) % Inode::ADDRESS_PER_INDEX_BLOCK;
            return lbns[indirectLbn];
        }

        return -1;
    };
}