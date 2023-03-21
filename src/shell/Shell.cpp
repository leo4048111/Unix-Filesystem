#include "Log.hpp"
#include "Shell.hpp"
#include "FileManager.hpp"

#include <iostream>
#include <string.h>

namespace ufs
{
    std::unique_ptr<Shell> Shell::_inst;

    Shell::Shell(){};

    Shell::~Shell(){};

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
        if (s == "ls")
            return InstCode::LS;
        if (s == "mkdir")
            return InstCode::MKDIR;
        if (s == "cd")
            return InstCode::CD;
        if (s == "touch")
            return InstCode::TOUCH;
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
        case InstCode::LS:
            return FileManager::getInstance()->ls();
        case InstCode::MKDIR:
        {
            if (_splitCmd.size() != 2)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->mkdir(_splitCmd[1]);
        }
        case InstCode::CD:
        {
            if (_splitCmd.size() != 2)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->cd(_splitCmd[1]);
        }
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

            if (ec == Error::UFS_NOERR)
                UFS_INFO("OK");
            else if (ec == Error::UFS_ERR_INVALID_COMMAND_ARG)
                UFS_ERROR("Invalid arguments");
            else if (ec == Error::UFS_ERR_NOT_MOUNTED)
                UFS_ERROR("File system not mounted");
            else if (ec == Error::UFS_ERR_NOT_A_DIR)
                UFS_ERROR("Not a directory");
            else if (ec == Error::UFS_ERR_NO_SUCH_DIR_OR_FILE)
                UFS_ERROR("No such file or directory");

            fflush(stdin);
        }
    }
}