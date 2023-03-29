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
        Error cat(const std::string &fileName);
        Error rm(const std::string &fileName);
        Error rmdir(const std::string& dirName);
        Error cp(const std::string& srcName, const std::string& dstName); // srcName must be an existing file outside of this file system
        Error truncate(const std::string& fileName, int newSize); // truncate the file to newSize bytes
        Error tail(const std::string& fileName, int offset, int size, const std::string &newFileName);

        bool isMounted() const { return _isMounted; }

        const std::string curPath() const { return _curPath; }

    private:
        int _curDirInodeNo{0};
        bool _isMounted{false};
        std::string _curPath{"/"};
    };
}