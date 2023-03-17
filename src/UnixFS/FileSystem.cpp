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

    void FileSystem::loadSuperBlock(SuperBlock &superblock)
    {
        Buf *bp = BufferManager::getInstance()->bread(0);
        memcpy_s(&superblock, DISK_BLOCK_SIZE, bp->b_addr, DISK_BLOCK_SIZE);
        BufferManager::getInstance()->brelse(bp);
    }

    void FileSystem::writeInodeCacheBackToDisk(Inode &inode)
    {
        // convert Inode to DiskInode
        DiskInode dInode(inode);

        // get the block number which should be written back to disk
        int blkno = 1 + inode.i_number / (DISK_BLOCK_SIZE / DISKINODE_SIZE);
        Buf *bp = BufferManager::getInstance()->bread(blkno);
        // get the starting address of the inode in the block
        uintptr_t destAddr = (uintptr_t)bp->b_addr;
        destAddr = destAddr + (inode.i_number % (DISK_BLOCK_SIZE / DISKINODE_SIZE)) * DISKINODE_SIZE;
        // memcpy the DiskInode to the block, then call bwrite to write buffer back to disk
        memcpy_s((void*)destAddr, DISKINODE_SIZE, &dInode, DISKINODE_SIZE);
        BufferManager::getInstance()->bdwrite(bp);
    }

    Error FileSystem::mount()
    {
        Error ec = BufferManager::getInstance()->mount();
        if (ec == Error::UFS_ERR_MOUNT_FAILED)
        {
            _fsStat = FS_STATUS::FS_UNINITIALIZED;
            return ec;
        }

        // image mounted successfully, but not formatted
        if (ec == Error::UFS_IMAGE_NO_FORMAT)
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

    Error FileSystem::unmount()
    {
        Error ec = BufferManager::getInstance()->unmount();

        if(ec == Error::UFS_NOERR)
            _fsStat = FS_STATUS::FS_UNINITIALIZED;

        return ec;
    }
}