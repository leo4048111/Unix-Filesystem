#include "Shell.hpp"

#include "InodeTable.hpp"

int main()
{
    int tmp = sizeof(ufs::InodeTable);
    ufs::Shell::getInstance()->loop();
    return 0;
}