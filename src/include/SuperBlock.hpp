#pragma once

#include "Defines.hpp"
#include <string.h>

namespace ufs
{
    class SuperBlock
    {
    public:
        SuperBlock();

        ~SuperBlock();

        SuperBlock(const SuperBlock& block) {
            memcpy_s(this, sizeof(SuperBlock), &block, sizeof(SuperBlock));
        }

        SuperBlock& operator=(const SuperBlock& block) {
            memcpy_s(this, sizeof(SuperBlock), &block, sizeof(SuperBlock));
            return *this;
        }

        size_t SuperBlockBlockNum = 1;  // 暂时考虑superblock占1个磁盘block
        int free_inode_num;             // 空闲inode
        int free_block_bum;             // 空闲盘块数
        int total_block_num;            // 总盘块数
        int total_inode_num;            // 总inode数
        int s_inode[MAX_INODE_NUM - 1]; // 空闲inode栈，用于管理inode的分配和回收
        int s_ninode;                   // 直接管理的空闲外存Inode数量
        char padding[3560];

        int balloc();
        void bfree(int blkNum);
        void bsetOccupy(int blkNum);
        int ialloc();
        void ifree(int inodeId);
    };
}