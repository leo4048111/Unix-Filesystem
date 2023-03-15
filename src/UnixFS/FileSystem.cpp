#include "FileSystem.hpp"
#include "BufferManager.hpp"
#include "SuperBlockManager.hpp"

namespace ufs
{
    std::unique_ptr<FileSystem> FileSystem::_inst;

    FileSystem::FileSystem()
    {
    }

    FileSystem::~FileSystem()
    {
    }

    void FileSystem::loadSuperBlock(SuperBlock& superblock)
    {
        Buf* bp = BufferManager::getInstance()->bread(0);
        memcpy_s(&superblock, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
        BufferManager::getInstance()->brelse(bp);
    }

    Error FileSystem::mount()
    {
        Error ec = BufferManager::getInstance()->mount();
        if (ec == Error::UFS_ERR_MOUNT_FAILED) {
            _fsStat = FS_STATUS::FS_UNINITIALIZED;
            return ec;
        }

        // image mounted successfully, but not formatted
        if(ec == Error::UFS_IMAGE_NO_FORMAT)
        {
            _fsStat = FS_STATUS::FS_NOFORM;
            return Error::UFS_NOERR;
        }

        // image mounted and formatted
        _fsStat = FS_STATUS::FS_READY;

        SuperBlock tmpSB;
        loadSuperBlock(tmpSB);

        SuperBlockManager::getInstance()->setSuperBlock(tmpSB);
        SuperBlockManager::getInstance()->setDirty(false);

        return ec;
    }
}