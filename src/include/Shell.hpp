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
        enum class InstCode
        {
            INVALID = 0,
            MOUNT,
            UNMOUNT
        };

        char _ttyBuffer[TTY_BUFFER_SIZE];
        std::vector<std::string> _splitCmd;
        int _cmdSeqNum = 0;

        void printPrefix();

        void parseCmdLiteral();

        Error runCmd(InstCode code);

        InstCode cmdLiteralToInstCode(const std::string& s);

    };
}