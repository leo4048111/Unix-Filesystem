#pragma once

#include "Log.hpp"
#include "Defines.hpp"

#include <string.h>

namespace ufs
{
    class SuperBlock
    {
    public:
        SuperBlock();

        ~SuperBlock();

        SuperBlock(const SuperBlock &block)
        {
            memcpy_s(this, sizeof(SuperBlock), &block, sizeof(SuperBlock));
        }

        SuperBlock &operator=(const SuperBlock &block)
        {
            memcpy_s(this, sizeof(SuperBlock), &block, sizeof(SuperBlock));
            return *this;
        }

        int s_isize; /* 外存Inode区占用的盘块数 */
        int s_fsize; /* 盘块总数 */

        int s_nfree;     /* 直接管理的空闲盘块数量 */
        int s_free[100]; /* 直接管理的空闲盘块索引表 */

        int s_ninode;     /* 直接管理的空闲外存Inode数量 */
        int s_inode[100]; /* 直接管理的空闲外存Inode索引表 */

        int s_flock; /* 封锁空闲盘块索引表标志 */
        int s_ilock; /* 封锁空闲Inode表标志 */

        int s_fmod;         /* 内存中super block副本被修改标志，意味着需要更新外存对应的Super Block */
        int s_ronly;        /* 本文件系统只能读出 */
        int s_time;         /* 最近一次更新时间 */
        char padding[3260]; /* 填充使SuperBlock块大小等于4096字节，占据1个盘块 */

        int balloc(); // allocate a free disk blkno
        int ialloc(); // allocate a free inode
    };
}