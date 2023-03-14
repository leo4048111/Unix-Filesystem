#pragma once

#include <memory>

#include "Defines.hpp"
#include "ImageMap.hpp"
#include "DiskBlock.hpp"

namespace ufs
{
    class DiskDriver
    {
    private:
        DiskDriver() = default;
        DiskDriver(const DiskDriver &) = delete;
        DiskDriver &operator=(const DiskDriver &) = delete;

        static std::unique_ptr<DiskDriver> _inst;

    public:
        static DiskDriver *getInstance()
        {
            if (_inst.get() == nullptr)
                _inst.reset(new DiskDriver);

            return _inst.get();
        }

    public:
        ~DiskDriver() = default;

        Error mount();  // mount disk to file img

        Error unmount(); // unmount disk from file img

        Error writeBlk(int blkno, const DiskBlock &blk); // write a disk block to disk

    private:
        std::unique_ptr<ImageMap> _imgMap {nullptr};
    };
}