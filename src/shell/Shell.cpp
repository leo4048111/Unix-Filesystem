#include "../include/Shell.hpp"

#include <iostream>
#include <string.h>

namespace ufs
{
    std::unique_ptr<Shell> Shell::_inst;

    void Shell::loop()
    {
        while (true)
        {
            putchar('$');
            putchar(' ');

            std::cin.getline(_ttyBuffer, MAX_CMD_LEN, '\n');

            for (char *checker = strrchr(_ttyBuffer, '\t'); checker != NULL; checker = strrchr(checker, '\t'))
                *checker = ' ';

            char *tmpBuffer = strdup(_ttyBuffer);

            memset(_splitCmd, 0, sizeof(_splitCmd));
            int cmdSeqNum = 0;
            for (char *p = strtok(tmpBuffer, " "); p != nullptr; p = strtok(NULL, " "), cmdSeqNum++)
            {
                strcpy(_splitCmd[cmdSeqNum], p);
            }
            _cmdSeqNum = cmdSeqNum;

            //parseCmd();
            delete tmpBuffer;
            fflush(stdin);
        }
    }
}