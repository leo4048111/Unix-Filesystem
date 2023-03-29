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
        std::cout << rang::style::bold << rang::fg::green << "Admin@DESKTOP-55513A7 " << rang::style::reset;
        std::cout << rang::style::bold << rang::fg::yellow << FileManager::getInstance()->curPath() << rang::style::reset << std::endl;
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

    Shell::InstCode Shell::cmdLiteralToInstCode(std::string &s)
    {
        for(int i = 0; i < s.size(); i++)
            s[i] = std::tolower(s[i]);

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
        if (s == "echo")
            return InstCode::ECHO;
        if (s == "cat")
            return InstCode::CAT;
        if (s == "rm")
            return InstCode::RM;
        if (s == "rmdir")
            return InstCode::RMDIR;
        if (s == "cp")
            return InstCode::CP;
        if (s == "truncate")
            return InstCode::TRUNCATE;
        if (s == "tail")
            return InstCode::TAIL;

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
        case InstCode::TOUCH:
        {
            if (_splitCmd.size() != 2)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->touch(_splitCmd[1]);
        }
        case InstCode::ECHO:
        {
            if (_splitCmd.size() == 2)
                return FileManager::getInstance()->echo(_splitCmd[1]);

            else if (_splitCmd.size() != 4 || _splitCmd[2] != ">")
                return Error::UFS_ERR_INVALID_COMMAND_ARG;

            return FileManager::getInstance()->echo(_splitCmd[3], _splitCmd[1]);
        }
        case InstCode::CAT:
        {
            if (_splitCmd.size() != 2)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->cat(_splitCmd[1]);
        }
        case InstCode::RM:
        {
            if (_splitCmd.size() != 2)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->rm(_splitCmd[1]);
        }
        case InstCode::RMDIR:
        {
            if (_splitCmd.size() != 2)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->rmdir(_splitCmd[1]);
        }
        case InstCode::CP:
        {
            if (_splitCmd.size() != 3)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->cp(_splitCmd[1], _splitCmd[2]);
        }
        case InstCode::TRUNCATE:
        {
            if (_splitCmd.size() != 4)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            if (_splitCmd[1] != "-s")
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->truncate(_splitCmd[2], std::stoi(_splitCmd[3]));
        }
        case InstCode::TAIL:
        {
            if (_splitCmd.size() != 7)
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            if (_splitCmd[1] != "-c")
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            if (_splitCmd[5] != ">")
                return Error::UFS_ERR_INVALID_COMMAND_ARG;
            return FileManager::getInstance()->tail( _splitCmd[4], std::stoi(_splitCmd[2]), std::stoi(_splitCmd[3]), _splitCmd[6]);
        }
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