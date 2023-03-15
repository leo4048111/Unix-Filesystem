#pragma once

#include "rang.hpp"

#include <memory>
#include <string>
#include <vector>

#include "Defines.hpp"

namespace ufs
{
    class Shell
    {
        SINGLETON(Shell)
    public:
        void loop();

        void error(const std::string msg);

        void warning(const std::string msg);

        void info(const std::string msg);

    private:
        enum class InstCode
        {
            INVALID = 0,
            MOUNT,
            UNMOUNT,
            FORMAT,
            CD
        };

        char _ttyBuffer[TTY_BUFFER_SIZE];
        std::vector<std::string> _splitCmd;
        int _cmdSeqNum = 0;

        void printPrefix();

        void parseCmdLiteral();

        Error runCmd(InstCode code);

        InstCode cmdLiteralToInstCode(const std::string &s);
    };
}