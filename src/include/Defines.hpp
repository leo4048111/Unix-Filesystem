#pragma once

namespace ufs
{
#define SINGLETON(classname)                          \
private:                                              \
    classname();                                    \
    classname(const classname &) = delete;            \
    classname &operator=(const classname &) = delete; \
                                                      \
    static std::unique_ptr<classname> _inst;          \
                                                      \
public:                                               \
    static classname *getInstance()                   \
    {                                                 \
        if (_inst.get() == nullptr)                   \
            _inst.reset(new classname);               \
                                                      \
        return _inst.get();                           \
    }                                                 \
                                                      \
public:                                               \
    ~classname();

    using BYTE = uint8_t;

    enum class Error
    {
        UFS_NOERR = 0,
        UFS_ERR_MOUNT_FAILED,
        UFS_CMD_NOT_FOUND
    };

#define TTY_BUFFER_SIZE 4096
#define MAX_CMD_LEN 4096
#define DISK_IMG_FILEPATH "D:\\Projects\\CPP\\Homework\\Unix-FileSystem\\build\\disk.img"
#define DISK_BLOCK_SIZE 4096
#define DISK_SIZE (64 * 1024 * 1024)
#define DISK_BLOCK_NUM (DISK_SIZE / DISK_BLOCK_SIZE)
}