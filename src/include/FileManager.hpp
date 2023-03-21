#pragma once

#include "Log.hpp"
#include "Defines.hpp"

namespace ufs
{
    class FileManager
    {
        SINGLETON(FileManager)

    public:
        Error mount();
        Error unmount();
        Error format();
        Error ls();
        Error mkdir(const std::string& dirName, bool isRoot = false);
        Error cd(const std::string& dirName);

        bool isMounted() const { return _isMounted; }

    private:
        int _curDirInodeNo{0};
        bool _isMounted{false};
    };
}