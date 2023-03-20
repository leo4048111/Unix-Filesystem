#include "ImageMap.hpp"

namespace ufs
{
    ImageMap::ImageMap(const std::string &path)
    {
        _fs.open(path, std::fstream::in | std::fstream::out | std::fstream::binary);

        if (!_fs)
        {
            _fs.open(path, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
        }

        _path = path;
        _fs.seekg(0, std::ios::beg);
        _fs.seekp(0, std::ios::beg);
    }

    ImageMap::~ImageMap()
    {
        _fs.close();
    }

    const size_t ImageMap::size()
    {
        size_t fsize = _fs.tellg();
        _fs.seekg(0, std::ios::end);
        return (size_t)_fs.tellg() - fsize;
    }
}
