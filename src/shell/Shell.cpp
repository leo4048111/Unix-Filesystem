#include "Shell.hpp"

#include <iostream>
#include <string.h>

#include "DiskDriver.hpp"

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

    void Shell::error(const std::string msg)
    {
        // std::cout << rang::style::bold << rang::fg::red << "Error: " << rang::style::reset << msg << std::endl;
        std::cout << "Error: " << msg << std::endl;
    }

    void Shell::warning(const std::string msg)
    {
        // std::cout << rang::style::bold << rang::fg::yellow << "Warning: " << rang::style::reset << msg << std::endl;
        std::cout << "Warning: " << msg << std::endl;
    }

    void Shell::info(const std::string msg)
    {
        // std::cout << rang::style::bold << rang::fg::green << "Info: " << rang::style::reset << msg << std::endl;
        std::cout << "Info: " << msg << std::endl;
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
            return DiskDriver::getInstance()->mount();
        case InstCode::UNMOUNT:
            return DiskDriver::getInstance()->unmount();
        case InstCode::FORMAT:
            return Error::UFS_NOERR;
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

        warning("Command not found.");
        return Error::UFS_CMD_NOT_FOUND;
    }

    void Shell::loop()
    {
        while (true)
        {
            printPrefix();
            parseCmdLiteral();
            runCmd(cmdLiteralToInstCode(this->_splitCmd[0]));
            fflush(stdin);
        }
    }
}