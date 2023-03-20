#pragma once

#include <cstdint>
#include <memory>

namespace ufs
{
#define SINGLETON(classname)                          \
private:                                              \
    classname();                                      \
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

#define INCONSTRUCTIBLE(classname)    \
    classname() = delete;             \
    classname(classname &&) = delete; \
    classname(const classname &) = delete;

    using BYTE = uint8_t;

    enum class Error
    {
        UFS_NOERR = 0,
        UFS_ERR_MOUNT_FAILED,
        UFS_ERR_INVALID_COMMAND_ARG,
        UFS_ERR_NOT_MOUNTED,
        UFS_ERR_ALREADY_MOUNTED,
        UFS_IMAGE_NO_FORMAT,
        UFS_CMD_NOT_FOUND
    };

#define TTY_BUFFER_SIZE 4096ULL
#define MAX_CMD_LEN 4096ULL
#define DISK_IMG_FILEPATH "D:\\Projects\\CPP\\Homework\\Unix-FileSystem\\build\\disk.img"
#define DISK_BLOCK_SIZE 4096ULL
#define DISK_SIZE (64 * 1024 * 1024)
#define DISK_BLOCK_NUM (DISK_SIZE / DISK_BLOCK_SIZE)
#define DISKINODE_SIZE 64ULL
#define MAX_INODE_NUM (3 * DISK_BLOCK_SIZE / DISKINODE_SIZE)
#define MIXED_ADDR_TABLE_SIZE (10 * sizeof(int))
}