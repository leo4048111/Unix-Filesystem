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
        Error mkdir(const std::string &dirName, bool isRoot = false);
        Error cd(const std::string &dirName);
        Error touch(const std::string &fileName);
        Error echo(const std::string &msg);
        Error echo(const std::string &fileName, const std::string &data);

        bool isMounted() const { return _isMounted; }

        const std::string curPath() const { return _curPath; }

    private:
        int _curDirInodeNo{0};
        bool _isMounted{false};
        std::string _curPath{"/"};
    };
}