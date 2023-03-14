#include "ImageMap.hpp"

namespace ufs
{
    ImageMap::ImageMap(const std::string& path)
    {
        _fs.open(path, std::fstream::in | std::fstream::out | std::fstream::binary);

        if(!_fs)
        {
            _fs.open(path, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
        }

        _path = path;
    }

    ImageMap::~ImageMap()
    {
        _fs.close();
    }
}
