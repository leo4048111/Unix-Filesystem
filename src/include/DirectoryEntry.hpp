#pragma once

namespace ufs
{
    class DirectoryEntry
    {
        /* static members */
    public:
        static const int DIRSIZ = 28; /* 目录项中路径部分的最大字符串长度 */

        /* Functions */
    public:
        /* Constructors */
        // DirectoryEntry(int m_ino,char *name);
        DirectoryEntry() = default;
        /* Destructors */
        ~DirectoryEntry() = default;

        /* Members */
    public:
        int _ino;           /* Inode number */
        char _name[DIRSIZ]; /* directory path */
    };
}