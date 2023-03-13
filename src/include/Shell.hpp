#pragma once

#include <memory>

#define TTY_BUFFER_SIZE 4096
#define MAX_CMD_LEN 4096
#define MAX_PARAM_NUM 32
#define MAX_SINGLE_PARAM_LEN 128

namespace ufs
{
    class Shell
    {
    private:
        Shell() = default;
        Shell(const Shell &) = delete;
        Shell &operator=(const Shell &) = delete;

        static std::unique_ptr<Shell> _inst;

        char _ttyBuffer[TTY_BUFFER_SIZE];
        char _splitCmd[MAX_PARAM_NUM][MAX_SINGLE_PARAM_LEN]{};
        int _cmdSeqNum = 0;

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
    };
}