#pragma once

#include "rang.hpp"

#include <memory>
#include <string>
#include <vector>

#define TTY_BUFFER_SIZE 4096
#define MAX_CMD_LEN 4096

namespace ufs
{
    class Shell
    {
    private:
        Shell() = default;
        Shell(const Shell &) = delete;
        Shell &operator=(const Shell &) = delete;

        static std::unique_ptr<Shell> _inst;

    public:
        static Shell *getInstance()
        {
            if (_inst.get() == nullptr)
                _inst.reset(new Shell);

            return _inst.get();
        }

    public:
        ~Shell() = default;

        void loop();

        void error(const std::string msg);

        void warning(const std::string msg);

        void info(const std::string msg);

    private:
        char _ttyBuffer[TTY_BUFFER_SIZE];
        std::vector<std::string> _splitCmd;
        int _cmdSeqNum = 0;

        void printPrefix();

        void parseCmdLiteral();

        bool runCmd();
    };
}