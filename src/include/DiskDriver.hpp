#pragma once

#include <memory>

#include "Defines.hpp"
#include "ImageMap.hpp"
#include "DiskBlock.hpp"

namespace ufs
{
    class DiskDriver
    {
        SINGLETON(DiskDriver)

    public:
        Error mount(); // mount disk to file img

        Error unmount(); // unmount disk from file img

        Error writeBlk(int blkno, const DiskBlock &blk); // write a disk block to disk

        Error readBlk(int blkno, DiskBlock& blk); // read a disk block from disk

        int devno() const { return _devno; } // get device number
    private:
        int _devno { 22 }; // device number

        std::unique_ptr<ImageMap> _imgMap{nullptr};
    };
}