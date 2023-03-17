#include "Inode.hpp"

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
        return 0;
    };
}