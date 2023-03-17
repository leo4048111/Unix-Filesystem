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
        Error clear(); // clear disk

        Error mount(); // mount disk to file img

        Error unmount(); // unmount disk from file img

        Error writeBlk(int blkno, const DiskBlock &blk); // write a disk block to disk

        Error readBlk(int blkno, DiskBlock& blk); // read a disk block from disk

        template<typename T>
        Error writeOffset(const size_t offset, T& elem); // write to offset from the start of image file

        template<typename T>
        Error readOffset(const size_t offset, T& elem); // read from offset from the start of image file

        int devno() const { return _devno; } // get device number
    private:
        int _devno { 22 }; // device number

        std::unique_ptr<ImageMap> _imgMap{nullptr};
    };

    template<typename T>
    Error DiskDriver::writeOffset(const size_t offset, T& elem)
    {
        return _imgMap->write(offset, elem);
    }

    template<typename T>
    Error DiskDriver::readOffset(const size_t offset, T& elem)
    {
        return _imgMap->read(offset, elem);
    }
}