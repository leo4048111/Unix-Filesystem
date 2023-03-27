#include "Log.hpp"

#include "FileManager.hpp"
#include "FileSystem.hpp"
#include "InodeTable.hpp"
#include "SuperBlockManager.hpp"
#include "SuperBlock.hpp"
#include "BufferManager.hpp"
#include "DirectoryEntry.hpp"
#include "DiskDriver.hpp"

#include <time.h>
#include <fstream>

namespace ufs
{
    std::unique_ptr<FileManager> FileManager::_inst;

    FileManager::FileManager()
    {
    }

    FileManager::~FileManager()
    {
    }

    Error FileManager::mount()
    {
        // Steps to mount the disk
        // (1) initialize memory InodeCache with InodeCache::clearCache()
        // (2) Open the disk image file with DiskDriver::mount()
        // (3) Load SuperBlock to SuperBlock cache

        if (isMounted())
        {
            Error ec = Error::UFS_ERR_ALREADY_MOUNTED;
            return ec;
        }

        InodeTable::getInstance()->clear();

        Error ec = FileSystem::getInstance()->mount();

        if (ec == Error::UFS_NOERR)
            _isMounted = true;

        _curDirInodeNo = 0;
        _curPath = "/";

        return ec;
    }

    Error FileManager::unmount()
    {
        Error ec = Error::UFS_NOERR;
        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to unmount the disk
        // (1) Flush all dirty InodeCache to disk with InodeCache::flushAllDirtyInodeCache()
        // (2) Flush SuperBlock cache to disk with SuperBlock::flushSuperBlock()
        // (3) Close the disk image file with DiskDriver::unmount()
        InodeTable::getInstance()->flushAllDirtyInodeCache();

        if (SuperBlockManager::getInstance()->isDirty())
        {
            SuperBlockManager::getInstance()->flushSuperBlockCache();
        }

        ec = FileSystem::getInstance()->unmount();

        if (ec == Error::UFS_NOERR)
            _isMounted = false;

        return ec;
    }

    Error FileManager::touch(const std::string &fileName)
    {
        Error ec = Error::UFS_NOERR;
        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to create a file
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Allocate a new inode with InodeTable::ialloc()
        // (3) Allocate a new data block with SuperBlock::balloc()
        // (4) Add a new directory entry to current directory inode

        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);
        Inode newFileInode;
        newFileInode.i_mode = Inode::IFCHR;
        newFileInode.i_nlink = 1;
        newFileInode.i_count = 1;
        newFileInode.i_size = 0;

        // Allocate a new inode
        SuperBlock &sb = SuperBlockManager::getInstance()->superBlock();
        int newDirInodeNo = sb.ialloc();
        newFileInode.i_number = newDirInodeNo;
        newFileInode.i_addr[0] = sb.balloc();
        InodeTable::getInstance()->iupdate(newFileInode.i_number, newFileInode);

        // add directory entry for current file
        ec = InodeTable::getInstance()->addDirectoryEntry(_curDirInodeNo, fileName, newFileInode.i_number);

        return ec;
    }

    Error FileManager::echo(const std::string &msg)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Simply echo the msg
        Log::out(msg);
        Log::out("\n");

        return ec;
    }

    Error FileManager::echo(const std::string &fileName, const std::string &data)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to write data to a file
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Check if a directory entry with valid filename exists
        // (3) Get the inode for the file
        // (4) Write data to disk block pointed by the file inode's i_addr

        // Find current directory inode
        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);
        std::vector<BYTE> buffer;
        for (auto &c : data)
            buffer.push_back(c);

        for (size_t i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry); i++)
        {
            DirectoryEntry &dirEntry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(dirEntry._name, fileName.c_str()) == 0)
            {
                Inode &fileInode = InodeTable::getInstance()->iget(dirEntry._ino);
                if (fileInode.i_mode != Inode::IFCHR)
                {
                    ec = Error::UFS_ERR_NOT_A_FILE;
                    return ec;
                }

                ec = FileSystem::getInstance()->fwrite(fileInode, buffer);
                InodeTable::getInstance()->iupdate(fileInode.i_number, fileInode);
                return ec;
            }
        }

        ec = Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
        return ec;
    }

    Error FileManager::ls()
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to list all files in current directory
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Read all directory entries in current directory inode with Inode::read()
        // (3) Print all directory entries' file name

        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);

        // traverse every directory entry
        for (size_t i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry); ++i)
        {
            DirectoryEntry &dirEntry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            std::string name = dirEntry._name;
            name += " \n"[i == curDirInode.i_size / sizeof(DirectoryEntry) - 1];
            Inode &inode = InodeTable::getInstance()->iget(dirEntry._ino);
            if (inode.i_mode == Inode::IFDIR)
                Log::out(name, rang::fg::blue, rang::style::bold);
            else
                Log::out(name, rang::fg::green, rang::style::bold);
        }

        return ec;
    }

    Error FileManager::cd(const std::string &dirName)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // steps to change current directory
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Read all directory entries in current directory inode with Inode::read()
        // (3) Find the directory entry with the given directory name
        // (4) Check if the directory entry is a directory
        // (5) Change current directory inode number to the directory entry's inode number

        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);
        for (int i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry); ++i)
        {
            DirectoryEntry &dirEntry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(dirEntry._name, dirName.c_str()) == 0)
            {
                const Inode &dirInode = InodeTable::getInstance()->iget(dirEntry._ino);
                if (dirInode.i_mode != Inode::IFDIR)
                    return Error::UFS_ERR_NOT_A_DIR;

                _curDirInodeNo = dirEntry._ino;
                _curPath += dirName + "/";
                return Error::UFS_NOERR;
            }
        }

        ec = Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
        return ec;
    }

    Error FileManager::cat(const std::string &fileName)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // steps to read data from a file
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Check if a directory entry with valid filename exists
        // (3) Get the inode for the file
        // (4) Read data from disk block pointed by the file inode's i_addr

        // Find current directory inode
        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);

        for (size_t i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry); i++)
        {
            DirectoryEntry &dirEntry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(dirEntry._name, fileName.c_str()) == 0)
            {
                Inode &fileInode = InodeTable::getInstance()->iget(dirEntry._ino);
                if (fileInode.i_mode != Inode::IFCHR)
                {
                    ec = Error::UFS_ERR_NOT_A_FILE;
                    return ec;
                }

                std::vector<BYTE> buffer;
                ec = FileSystem::getInstance()->fread(fileInode, buffer);
                buffer.push_back('\n');
                std::string s;
                for (auto &c : buffer)
                    s += c;
                Log::out(s);
                return ec;
            }
        }

        ec = Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
        return ec;
    }

    Error FileManager::mkdir(const std::string &dirName, bool isRoot)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // Steps to create a new directory
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Allocate a new inode with InodeTable::ialloc()
        // (3) Write directory entry to data block pointed by i_addr

        Inode newDirInode;
        newDirInode.i_mode = Inode::IFDIR;
        newDirInode.i_nlink = 1;
        newDirInode.i_count = 1;

        // Allocate a new inode
        SuperBlock &sb = SuperBlockManager::getInstance()->superBlock();
        int newDirInodeNo = sb.ialloc();
        newDirInode.i_number = newDirInodeNo;
        newDirInode.i_addr[0] = sb.balloc();
        InodeTable::getInstance()->iupdate(newDirInode.i_number, newDirInode);

        // Initialize directory entry
        InodeTable::getInstance()->addDirectoryEntry(newDirInodeNo, ".", newDirInodeNo);

        if (!isRoot)
            ec = InodeTable::getInstance()->addDirectoryEntry(newDirInodeNo, "..", _curDirInodeNo);

        else
            ec = InodeTable::getInstance()->addDirectoryEntry(newDirInodeNo, "..", newDirInodeNo);

        // update current directory inode
        if (!isRoot)
        {
            ec = InodeTable::getInstance()->addDirectoryEntry(_curDirInodeNo, dirName, newDirInodeNo);
        }

        return ec;
    }

    Error FileManager::format()
    {
        // Steps to format the disk
        // (1) clean the disk, set all bytes to 0
        // (2) initialize SuperBlock and flush it back to disk
        // (3) initialize InodeTable and flush it back to disk
        // (4) initialize Inodes' datablocks and flush them back to disk
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        DiskDriver::getInstance()->clear();

        // basic structure for disk.img
        // | superblock |  inodes   | data blocks              |
        // |     0#     |  1# ~ 3#  | 4# ~ (DISK_BLOCK_NUM-1)# |

        // load superblock in 0# disk block first
        SuperBlock tmpSB;
        tmpSB.s_isize = 3;
        tmpSB.s_fsize = DISK_BLOCK_NUM;
        tmpSB.s_fmod = true;
        tmpSB.s_time = time(NULL);

        // TODO Implement chain memory allocation
        for (tmpSB.s_nfree = 0; tmpSB.s_nfree < 500; tmpSB.s_nfree++)
            tmpSB.s_free[tmpSB.s_nfree] = 499 - tmpSB.s_nfree;

        // init inode table
        for (tmpSB.s_ninode = 0; tmpSB.s_ninode < 500; tmpSB.s_ninode++)
            tmpSB.s_inode[tmpSB.s_ninode] = 499 - tmpSB.s_ninode;

        // init root directory
        int superBlockDiskBlkno = tmpSB.balloc(); // alloc 0# diskblock to store superblock
        int inodepoolDiskBlkno[3] = {
            tmpSB.balloc(),
            tmpSB.balloc(),
            tmpSB.balloc()}; // aloc 1# ~ 3# diskblock to store inode pool

        SuperBlockManager::getInstance()->setSuperBlock(tmpSB);

        mkdir("root", true);
        mkdir("home");
        mkdir("usr");
        mkdir("bin");
        mkdir("etc");
        mkdir("dev");

        // Flush all new inodes back to disk
        InodeTable::getInstance()->flushAllDirtyInodeCache();

        // Flush SuperBlock to disk
        SuperBlockManager::getInstance()->flushSuperBlockCache();

        // Flush all dirty buffers back to disk
        BufferManager::getInstance()->flush();

        return ec;
    }

    Error FileManager::rm(const std::string &fileName)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // steps to remove a file
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Check if a directory entry with valid filename exists
        // (3) Get the inode for the file
        // (4) Remove data from disk block pointed by the file inode's i_addr
        // (5) free inode with SuperBlock::ifree(int inodeNo)
        // (6) remove the directory entry from current directory

        // Find current directory inode
        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);

        for (int i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry) + 1; i++)
        {
            DirectoryEntry &entry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(entry._name, fileName.c_str()) == 0)
            {
                Inode &fileInode = InodeTable::getInstance()->iget(entry._ino);
                if (fileInode.i_mode != Inode::IFCHR)
                {
                    ec = Error::UFS_ERR_NOT_A_FILE;
                    return ec;
                }

                // Free inode
                ec = FileSystem::getInstance()->freeInode(fileInode);

                // remove dir entry
                FileSystem::getInstance()->removeDirEntryAt(curDirInode, i);

                return ec;
            }
        }

        ec = Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
        return ec;
    }

    Error FileManager::rmdir(const std::string &dirName)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        // steps to remove a directory
        // (1) Get current directory inode with InodeTable::iget()
        // (2) Check if a directory entry with valid filename exists
        // (3) Get the inode for the directory
        // (4) Remove every sub file and sub directory recursively
        // (5) free inode with SuperBlock::ifree(int inodeNo)
        // (6) remove the directory entry from current directory

        // Find current directory inode
        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);

        for (int i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry) + 1; i++)
        {
            DirectoryEntry &entry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(entry._name, dirName.c_str()) == 0)
            {
                Inode &dirInode = InodeTable::getInstance()->iget(entry._ino);
                if (dirInode.i_mode != Inode::IFDIR)
                {
                    ec = Error::UFS_ERR_NOT_A_DIR;
                    return ec;
                }

                // Free inode
                ec = FileSystem::getInstance()->freeThisInodeAndAllSubInodes(dirInode);

                // remove dir entry
                FileSystem::getInstance()->removeDirEntryAt(curDirInode, i);
                return ec;
            }
        }

        ec = Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
        return ec;
    }

    Error FileManager::cp(const std::string &srcName, const std::string &dstName)
    {
        Error ec = Error::UFS_NOERR;

        if (!isMounted())
        {
            ec = Error::UFS_ERR_NOT_MOUNTED;
            return ec;
        }

        std::fstream fs;
        fs.open(srcName, std::fstream::in | std::fstream::binary);

        if (!fs.is_open())
        {
            ec = Error::UFS_ERR_NO_SUCH_DIR_OR_FILE;
            return ec;
        }

        std::vector<BYTE> buffer;
        char tmp;
        fs.seekg(0, std::ios::beg);
        while (fs.get(tmp))
        {
            BYTE byte = (BYTE)tmp;
            buffer.push_back(byte);
        }
        fs.close();

        Inode &curDirInode = InodeTable::getInstance()->iget(_curDirInodeNo);

        for (int i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry) + 1; i++)
        {
            DirectoryEntry &entry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(entry._name, dstName.c_str()) == 0)
            {
                ec = Error::UFS_ERR_FILE_ALREADY_EXISTS;
                return ec;
            }
        }

        ec = touch(dstName);
        int newFileInodeNo = -1;
        for (int i = 0; i < curDirInode.i_size / sizeof(DirectoryEntry) + 1; i++)
        {
            DirectoryEntry &entry = FileSystem::getInstance()->dirEntryAt(curDirInode, i);
            if (strcmp(entry._name, dstName.c_str()) == 0)
            {
                newFileInodeNo = entry._ino;
                break;
            }
        }

        Inode &newFileInode = InodeTable::getInstance()->iget(newFileInodeNo);
        FileSystem::getInstance()->fwrite(newFileInode, buffer);
        return ec;
    }
}