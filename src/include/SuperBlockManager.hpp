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

        bool isDirty() const { return _dirty; }

        void setDirty(bool dirty) { _dirty = dirty; }
        
    public:

        SuperBlock _superBlock;
        bool _dirty{false};
    };
}