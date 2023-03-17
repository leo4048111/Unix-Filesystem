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
        return s_free[--s_nfree];
    }

    int SuperBlock::ialloc()
    {
        return s_inode[--s_ninode];
    }
}