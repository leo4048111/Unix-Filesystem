#pragma once

#include <string>
#include "Defines.hpp"

namespace ufs
{
    class Log
    {
        INCONSTRUCTIBLE(Log)

    public:
        static void error(const std::string_view msg);

        static void warning(const std::string_view msg);

        static void info(const std::string_view msg);

        static void out(const std::string_view msg);
    };

#define UFS_ERROR(msg)        \
    do                        \
    {                         \
        ufs::Log::error(msg); \
    } while (0)

#define UFS_WARNING(msg)        \
    do                          \
    {                           \
        ufs::Log::warning(msg); \
    } while (0)

#define UFS_INFO(msg)        \
    do                       \
    {                        \
        ufs::Log::info(msg); \
    } while (0)

#define UFS_LOGOUT(msg)     \
    do                      \
    {                       \
        ufs::Log::out(msg); \
    } while (0)
}