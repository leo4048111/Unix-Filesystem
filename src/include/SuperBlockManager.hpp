#pragma once

#include "Defines.hpp"
#include "SuperBlock.hpp"

namespace ufs
{
    class SuperBlockManager
    {
        SINGLETON(SuperBlockManager)

        void setSuperBlock(const SuperBlock& superBlock)
        {
            _superBlock = superBlock;
        }

        void flushSuperBlockCache();

        SuperBlock& superBlock() {
            return _superBlock;
        }

        bool isDirty() const { return _superBlock.s_fmod; }
        
    public:

        SuperBlock _superBlock;
    };
}