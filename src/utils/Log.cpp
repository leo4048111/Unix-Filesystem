#include "Log.hpp"

namespace ufs
{
    void Log::error(const std::string_view msg)
    {
        std::cout << rang::fg::red << "Error: " << rang::style::reset << msg << std::endl;
        // std::cout << "Error: " << msg << std::endl;
    }

    void Log::warning(const std::string_view msg)
    {
        std::cout << rang::fg::yellow << "Warning: " << rang::style::reset << msg << std::endl;
        // std::cout << "Warning: " << msg << std::endl;
    }

    void Log::info(const std::string_view msg)
    {
        std::cout << rang::fg::green << "Info: " << rang::style::reset << msg << std::endl;
        // std::cout << "Info: " << msg << std::endl;
    }

    void Log::out(const std::string_view msg, rang::fg color, rang::style style)
    {
        std::cout << color << style << msg << rang::style::reset;
    }
}