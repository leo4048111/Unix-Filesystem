#pragma once

#include "Log.hpp"
#include "Defines.hpp"

#include <string.h>

namespace ufs
{
    class DiskBlock
    {
    public:
        DiskBlock()
        {
            memset(content, 0, sizeof(content));
        }

        BYTE content[DISK_BLOCK_SIZE]; // disk block content
    };
}