#include "DiskDriver.hpp"

namespace ufs
{
    std::unique_ptr<DiskDriver> DiskDriver::_inst;

    // mount disk to file img
    Error DiskDriver::mount()
    {
        Error ec = Error::UFS_NOERR;

        _imgMap.reset(new ImageMap(DISK_IMG_FILEPATH));

        if(_imgMap == nullptr) {
            ec = Error::UFS_ERR_MOUNT_FAILED;
            return ec;
        }

        _imgMap->write(0, 123);

        int test;
        _imgMap->read(0, test);
        std::cout << test << std::endl;
        return ec;
    }

    Error DiskDriver::unmount()
    {
        Error ec = Error::UFS_NOERR;

        return ec;
    }
}