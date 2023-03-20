#include "SuperBlock.hpp"

namespace ufs
{
    SuperBlock::SuperBlock()
    {
        memset(this, 0, sizeof(SuperBlock));
    }

    SuperBlock::~SuperBlock()
    {
    }

    int SuperBlock::balloc()
    {
        s_fmod = true;
        return s_free[--s_nfree];
    }

    int SuperBlock::ialloc()
    {
        s_fmod = true;
        return s_inode[--s_ninode];
    }
}