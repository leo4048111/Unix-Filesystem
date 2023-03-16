#include "SuperBlockManager.hpp"
#include "BufferManager.hpp"

namespace ufs
{
    std::unique_ptr<SuperBlockManager> SuperBlockManager::_inst;

    SuperBlockManager::SuperBlockManager()
    {
    }

    SuperBlockManager::~SuperBlockManager()
    {
    }

    void SuperBlockManager::flushSuperBlockCache()
    {
        // before writing to disk, read the block from disk to avoid stashing confliction
        Buf* buf = BufferManager::getInstance()->getBlk(0);
        memcpy_s(buf->b_addr, DISK_BLOCK_SIZE, &_superBlock, DISK_BLOCK_SIZE);
        BufferManager::getInstance()->bwrite(buf);
    }
}