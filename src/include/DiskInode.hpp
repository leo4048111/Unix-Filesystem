#pragma once

#include <string.h>

namespace ufs
{
    class Inode;

    class DiskInode
    {
        // 一个diskInode的大小为64B
    public:
        DiskInode()
        {
            memset(this, 0, sizeof(DiskInode));
        };

        DiskInode(Inode &inode); // 转换构造函数
        unsigned int d_mode;
        int d_nlink;
        short d_uid;
        short d_gid;
        int d_size;
        int d_addr[10]; // 混合索引表
        int d_atime;
        int d_mtime;

        // int inode_id;           //inode号
        // int uid;                //uid
        // size_t file_size;       //文件大小
        // FileType fileType;      //文件类型

        // DiskBlock *d_addr[10];  //混合索引表
    };
}