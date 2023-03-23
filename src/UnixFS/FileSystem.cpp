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
        superblock.s_fmod = false;

        UFS_DEBUG_INFO("Superblock loaded from disk");
    }

    void FileSystem::writeInodeCacheBackToDisk(Inode &inode)
    {
        UFS_DEBUG_INFO(Log::format("Preparing to write inode %d back to disk", inode.i_number));
        // convert Inode to DiskInode
        DiskInode dInode(inode);

        // get the block number which should be written back to disk
        int blkno = 1 + inode.i_number / (DISK_BLOCK_SIZE / DISKINODE_SIZE);
        Buf *bp = BufferManager::getInstance()->bread(blkno);

        DiskInode *destAddr = (DiskInode *)bp->b_addr;
        destAddr += (inode.i_number % (DISK_BLOCK_SIZE / DISKINODE_SIZE));

        // memcpy the DiskInode to the block, then call bwrite to write buffer back to disk
        memcpy_s(destAddr, DISKINODE_SIZE, &dInode, DISKINODE_SIZE);
        BufferManager::getInstance()->bdwrite(bp);
        inode.i_flag &= ~(Inode::INodeFlag::IUPD | Inode::INodeFlag::IACC);

        UFS_DEBUG_INFO(Log::format("Inode %d written back to disk", inode.i_number));
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

        return ec;
    }

    Error FileSystem::unmount()
    {
        Error ec = BufferManager::getInstance()->unmount();

        if (ec == Error::UFS_NOERR)
            _fsStat = FS_STATUS::FS_UNINITIALIZED;

        return ec;
    }

    DirectoryEntry &FileSystem::dirEntryAt(Inode &inode, int idx)
    {
        size_t totalSize = inode.i_size;

        size_t numDirectEntries = totalSize / sizeof(DirectoryEntry);

        size_t blkno = inode.bmap(idx / (DISK_BLOCK_SIZE / sizeof(DirectoryEntry)));
        Buf *bp = BufferManager::getInstance()->bread(blkno);
        DirectoryEntry *dirEntry = (DirectoryEntry *)bp->b_addr;

        dirEntry += (idx % (DISK_BLOCK_SIZE / sizeof(DirectoryEntry)));

        BufferManager::getInstance()->brelse(bp);

        return *dirEntry;
    }

    void FileSystem::removeDirEntryAt(Inode& inode, int idx)
    {
        size_t totalSize = inode.i_size;

        size_t numDirectEntries = totalSize / sizeof(DirectoryEntry);

        // move all the entries after the idx to the front
        for(int i = idx; i < numDirectEntries - 1; i++)
        {
            DirectoryEntry& dirEntry = dirEntryAt(inode, i);
            DirectoryEntry& nextDirEntry = dirEntryAt(inode, i + 1);

            dirEntry = nextDirEntry;
        }

        // update i_size
        inode.i_size -= sizeof(DirectoryEntry);
    }

    Error FileSystem::fwrite(Inode &inode, const std::string &buffer)
    {
        size_t totalSize = inode.i_size;
        int idx = totalSize / DISK_BLOCK_SIZE;
        int blkno = inode.bmap(idx);
        Buf *bp = BufferManager::getInstance()->bread(blkno);

        size_t offset = totalSize % DISK_BLOCK_SIZE;

        // TODO: if current available space in the block is not enough, we need to allocate a new block

        uintptr_t destAddr = (uintptr_t)bp->b_addr;
        destAddr += offset;
        memcpy_s((void *)destAddr, buffer.size(), buffer.c_str(), buffer.size());
        inode.i_size += buffer.size();

        BufferManager::getInstance()->bdwrite(bp);

        return Error::UFS_NOERR;
    }

    Error FileSystem::fread(Inode &inode, std::string &buffer)
    {
        size_t totalSize = inode.i_size;
        int maxIdx = totalSize / DISK_BLOCK_SIZE + 1;

        for (int i = 0; i < maxIdx; i++)
        {
            int blkno = inode.bmap(i);
            Buf *bp = BufferManager::getInstance()->bread(blkno);

            size_t sizeToRead = ((inode.i_size - i * DISK_BLOCK_SIZE) > DISK_BLOCK_SIZE ? DISK_BLOCK_SIZE : (inode.i_size - i * DISK_BLOCK_SIZE));

            buffer.append((char *)bp->b_addr, sizeToRead);
            BufferManager::getInstance()->brelse(bp);
        }

        return Error::UFS_NOERR;
    }

    Error FileSystem::freeInode(Inode &inode)
    {
        Error ec = Error::UFS_NOERR;

        size_t totalSize = inode.i_size;
        
        // Free every data block
        for(int i = 0; i < totalSize / DISK_BLOCK_SIZE + 1; i++)
        {
            int blkno = inode.bmap(i);
            SuperBlockManager::getInstance()->superBlock().bfree(blkno);
        }

        // Free inode
        SuperBlockManager::getInstance()->superBlock().ifree(inode.i_number);

        return ec;
    }
}