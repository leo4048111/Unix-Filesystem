#include "DiskDriver.hpp"

namespace ufs
{
    std::unique_ptr<DiskDriver> DiskDriver::_inst;

    DiskDriver::DiskDriver()
    {
    }

    DiskDriver::~DiskDriver()
    {
    }

    Error DiskDriver::clear()
    {
        Error ec = Error::UFS_NOERR;
        DiskBlock block;
        for (size_t i = 0; i < DISK_BLOCK_NUM; i++)
        {
            _imgMap->write(i * DISK_BLOCK_SIZE, block);
        }

        return ec;
    }

    // mount disk to file img
    Error DiskDriver::mount()
    {
        Error ec = Error::UFS_NOERR;

        _imgMap.reset(new ImageMap(DISK_IMG_FILEPATH));

        if (_imgMap == nullptr)
        {
            ec = Error::UFS_ERR_MOUNT_FAILED;
            return ec;
        }

        // If the disk img is empty, initialize it
        if (_imgMap->size() < DISK_SIZE)
        {
            clear();
            ec = Error::UFS_IMAGE_NO_FORMAT;
        }

        return ec;
    }

    Error DiskDriver::unmount()
    {
        Error ec = Error::UFS_NOERR;
        _imgMap.reset(nullptr);
        return ec;
    }

    Error DiskDriver::writeBlk(int blkno, const DiskBlock &blk)
    {
        return _imgMap->write(blkno * DISK_BLOCK_SIZE, blk);
    }

    Error DiskDriver::readBlk(int blkno, DiskBlock &blk)
    {
        return _imgMap->read(blkno * DISK_BLOCK_SIZE, blk);
    }
}