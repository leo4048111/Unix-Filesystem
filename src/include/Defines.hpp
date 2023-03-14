#pragma once

namespace ufs
{
    using BYTE = int8_t;

    enum class Error
    {
        UFS_NOERR = 0,
        UFS_ERR_MOUNT_FAILED,
        UFS_CMD_NOT_FOUND
    };

#define TTY_BUFFER_SIZE 4096
#define MAX_CMD_LEN 4096
#define DISK_IMG_FILEPATH "D:\\Projects\\CPP\\Homework\\Unix-FileSystem\\build\\disk.img"
}