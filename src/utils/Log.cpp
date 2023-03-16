#include "rang.hpp"
#include "Log.hpp"

namespace ufs
{
    void Log::error(const std::string msg)
    {
        // std::cout << rang::style::bold << rang::fg::red << "Error: " << rang::style::reset << msg << std::endl;
        std::cout << "Error: " << msg << std::endl;
    }

    void Log::warning(const std::string msg)
    {
        // std::cout << rang::style::bold << rang::fg::yellow << "Warning: " << rang::style::reset << msg << std::endl;
        std::cout << "Warning: " << msg << std::endl;
    }

    void Log::info(const std::string msg)
    {
        // std::cout << rang::style::bold << rang::fg::green << "Info: " << rang::style::reset << msg << std::endl;
        std::cout << "Info: " << msg << std::endl;
    }
}