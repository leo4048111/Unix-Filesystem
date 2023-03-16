#include "DiskInode.hpp"
#include "Inode.hpp"
#include "Defines.hpp"

namespace ufs
{
    class Inode;

    DiskInode::DiskInode(Inode &inode)
    {
        d_mode = inode.i_mode;
        d_nlink = inode.i_nlink;
        d_uid = inode.i_uid;
        d_gid = inode.i_gid;
        d_size = inode.i_size;
        memcpy(d_addr, inode.i_addr, MIXED_ADDR_TABLE_SIZE);
    }
}