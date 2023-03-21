#pragma once

#include "rang.hpp"
#include "Defines.hpp"

#include <string>

#define UFS_DEBUG

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

        template <typename... Args>
        static std::string format(const std::string &format, Args... args)
        {
            int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
            if (size_s <= 0)
            {
                throw std::runtime_error("Error during formatting.");
            }
            auto size = static_cast<size_t>(size_s);
            std::unique_ptr<char[]> buf(new char[size]);
            std::snprintf(buf.get(), size, format.c_str(), args...);
            return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
        }
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

#ifdef UFS_DEBUG
#define UFS_DEBUG_INFO(msg)  \
    do                       \
    {                        \
        ufs::Log::info(msg); \
    } while (0)
#else
#define UFS_DEBUG_INFO(msg) \
    do                      \
    {                       \
        ;                   \
    } while (0)
#endif
}
