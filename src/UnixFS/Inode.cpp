#include "Inode.hpp"

namespace ufs
{
    Inode::Inode(){};

    Inode::Inode(DiskInode diskInode){};
    
    int Inode::bmap(int lbn){ return 0; };
}