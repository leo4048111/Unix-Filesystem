#pragma once

namespace ufs
{
    enum class Error
    {
        UFS_NOERR = 0,
        UFS_ERR_MOUNT_FAILED,
        UFS_CMD_NOT_FOUND
    };

#define TTY_BUFFER_SIZE 4096
#define MAX_CMD_LEN 4096
#define DISK_IMG_FILEPATH "./disk.img"
}