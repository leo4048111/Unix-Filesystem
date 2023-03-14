#include "DiskDriver.hpp"

namespace ufs
{
    std::unique_ptr<DiskDriver> DiskDriver::_inst;

    // mount disk to file img
    Error DiskDriver::mount()
    {
        Error ec = Error::UFS_NOERR;

        // _mmapInst = std::make_unique<Mmap>(DISK_IMG_FILEPATH);

        // if (!_mmapInst->is_open())
        // {
        //     ec = Error::UFS_ERR_MOUNT_FAILED;
        //     return ec;
        // }

        return ec;
    }

    Error DiskDriver::unmount()
    {
        Error ec = Error::UFS_NOERR;

        return ec;
    }
}