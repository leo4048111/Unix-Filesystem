#include "rang.hpp"
#include "Log.hpp"
#include "Shell.hpp"

int main()
{
    UFS_DEBUG_INFO("UFS is running...");
    ufs::Shell::getInstance()->loop();
    return 0;
}