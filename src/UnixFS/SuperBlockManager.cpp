#include "SuperBlockManager.hpp"

namespace ufs
{
    std::unique_ptr<SuperBlockManager> SuperBlockManager::_inst;

    SuperBlockManager::SuperBlockManager()
    {
    }

    SuperBlockManager::~SuperBlockManager()
    {
    }
}