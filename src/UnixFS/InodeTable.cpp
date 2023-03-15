#include "InodeTable.hpp"

#include <string.h>

namespace ufs
{
    std::unique_ptr<InodeTable> InodeTable::_inst;

    InodeTable::InodeTable()
    {
    }

    InodeTable::~InodeTable()
    {
    }

    void InodeTable::clear()
    {
        // clear all inode cached in memory
        memset(_inodes, 0, sizeof(_inodes));
    }
}