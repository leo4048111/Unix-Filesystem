#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Defines.hpp"

namespace ufs
{
    class ImageMap
    {
    public:
        ImageMap() = default;
        ImageMap(const std::string &path);
        ~ImageMap();

        template <typename T>
        Error write(const size_t offset, T& elem);

        template <typename T>
        Error read(const size_t offset, T &elem);

        const size_t size();

    private:
        std::fstream _fs;
        std::string _path;
    };

    template <typename T>
    Error ImageMap::write(const size_t offset, T& elem)
    {
        Error ec = Error::UFS_NOERR;
        _fs.seekp(offset, std::ios::beg);
        _fs.write(reinterpret_cast<const char *>(&elem), sizeof(T));
        return ec;
    }

    template <typename T>
    Error ImageMap::read(const size_t offset, T &elem)
    {
        Error ec = Error::UFS_NOERR;
        _fs.seekg(offset, std::ios::beg);
        _fs.read(reinterpret_cast<char *>(&elem), sizeof(T));
        return ec;
    }
}
