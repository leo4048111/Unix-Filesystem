#include "../include/Shell.hpp"

#include <iostream>
#include <string.h>

namespace ufs
{
    std::unique_ptr<Shell> Shell::_inst;

    void Shell::printPrefix()
    {
        putchar('$');
        putchar(' ');
    }

    void Shell::parseCmdLiteral()
    {
        std::cin.getline(_ttyBuffer, MAX_CMD_LEN, '\n');

        for (char *checker = strrchr(_ttyBuffer, '\t'); checker != NULL; checker = strrchr(checker, '\t'))
            *checker = ' ';

        char *tmpBuffer = strdup(_ttyBuffer);

        _splitCmd.clear();
        for (char *p = strtok(tmpBuffer, " "); p != nullptr; p = strtok(NULL, " "))
        {
            _splitCmd.push_back(std::string(p));
        }

        delete tmpBuffer;
    }

    void Shell::error(const std::string msg)
    {
        std::cout << rang::style::bold << rang::fg::red << "Error: " << rang::style::reset << msg << std::endl;
    }

    void Shell::warning(const std::string msg)
    {
        std::cout << rang::style::bold << rang::fg::yellow << "Warning: " << rang::style::reset << msg << std::endl;
    }

    void Shell::info(const std::string msg)
    {
        std::cout << rang::style::bold << rang::fg::green << "Info: " << rang::style::reset << msg << std::endl; 
    }

    bool Shell::runCmd()
    {

        if(_splitCmd.size() == 0)
        {
            error("No command found.");
            return false;
        }

        std::string cmd = _splitCmd[0];

        switch(cmd)
        {
            case "mount":
                //mount();
                break;
            // case "unmount":
            //     unmount();
            //     break;
            // case "format":
            //     format();
            //     break;
            // case "cd":
            //     cd();
            //     break;
            // case "ls":
            //     ls();
            //     break;
            // case "rm":
            //     rm();
            //     break;
            // case "mkdir":
            //     mkdir();
            //     break;
        }


        return false;
    }

    void Shell::loop()
    {
        while (true)
        {
            printPrefix();
            parseCmdLiteral();
            runCmd();
            fflush(stdin);
        }
    }
}