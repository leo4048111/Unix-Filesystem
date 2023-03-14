#pragma once

#include "rang.hpp"

#include <memory>

#include "Defines.hpp"
#include "ImageMap.hpp"

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
    private:
        std::unique_ptr<ImageMap> _imgMap {nullptr};
    };
}