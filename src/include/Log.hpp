#include <string>

namespace ufs
{
    class Log
    {
    public:
        static void error(const std::string msg);

        static void warning(const std::string msg);

        static void info(const std::string msg);
    };
}