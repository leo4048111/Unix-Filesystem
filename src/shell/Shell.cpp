#include "Shell.hpp"

#include <iostream>
#include <string.h>

#include "FileManager.hpp"
#include "Log.hpp"
#include "nameof.hpp"

namespace ufs
{
    std::unique_ptr<Shell> Shell::_inst;

    Shell::Shell() {};

    Shell::~Shell() {};

    void Shell::printPrefix()
    {
        std::cout << rang::style::bold << rang::fg::green << "Admin@DESKTOP-55513A7 " << rang::fg::yellow << "~" << rang::style::reset << std::endl;
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

    Shell::InstCode Shell::cmdLiteralToInstCode(const std::string &s)
    {
        if (s == "mount")
            return InstCode::MOUNT;
        if (s == "unmount")
            return InstCode::UNMOUNT;
        if (s == "format")
            return InstCode::FORMAT;
        if (s == "cd")
            return InstCode::CD;
        return InstCode::INVALID;
    }

    Error Shell::runCmd(InstCode code)
    {
        switch (code)
        {
        case InstCode::MOUNT:
            return FileManager::getInstance()->mount();
        case InstCode::UNMOUNT:
            return FileManager::getInstance()->unmount();
        case InstCode::FORMAT:
            return FileManager::getInstance()->format();
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

        Log::warning("Command not found");
        return Error::UFS_CMD_NOT_FOUND;
    }

    void Shell::loop()
    {
        while (true)
        {
            printPrefix();
            parseCmdLiteral();
            Error ec = runCmd(cmdLiteralToInstCode(this->_splitCmd[0]));
            
            if(ec == Error::UFS_NOERR)
                UFS_INFO("OK");
            
            fflush(stdin);
        }
    }
}