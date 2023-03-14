#pragma once

#if defined(_WIN32)
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <stdexcept>

namespace ufs
{
    class Mmap
    {
    public:
        Mmap();
        Mmap(const char *path);
        ~Mmap();

        bool open(const char *path);
        void close();

        bool is_open() const;
        size_t size() const;
        const char *data() const;

    private:
#if defined(_WIN32)
        HANDLE hFile_;
        HANDLE hMapping_;
#else
        int fd_;
#endif
        size_t size_;
        void *addr_;
    };

#if defined(_WIN32)
#define MAP_FAILED NULL
#endif
    inline Mmap::Mmap()
#if defined(_WIN32)
        : hFile_(NULL), hMapping_(NULL)
#else
        : fd_(-1)
#endif
          ,
          size_(0), addr_(MAP_FAILED)
    {
    }

    inline Mmap::Mmap(const char *path)
#if defined(_WIN32)
        : hFile_(NULL), hMapping_(NULL)
#else
        : fd_(-1)
#endif
          ,
          size_(0), addr_(MAP_FAILED)
    {
        if (!open(path))
        {
            std::runtime_error("");
        }
    }

    inline Mmap::~Mmap() { close(); }

    inline bool Mmap::open(const char *path)
    {
        close();

#if defined(_WIN32)
        hFile_ = ::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile_ == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        size_ = ::GetFileSize(hFile_, NULL);

        hMapping_ = ::CreateFileMapping(hFile_, NULL, PAGE_READONLY, 0, 0, NULL);

        if (hMapping_ == NULL)
        {
            close();
            return false;
        }

        addr_ = ::MapViewOfFile(hMapping_, FILE_MAP_READ, 0, 0, 0);
#else
        fd_ = ::open(path, O_RDONLY);
        if (fd_ == -1)
        {
            return false;
        }

        struct stat sb;
        if (fstat(fd_, &sb) == -1)
        {
            close();
            return false;
        }
        size_ = sb.st_size;

        addr_ = ::Mmap(NULL, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
#endif

        if (addr_ == MAP_FAILED)
        {
            close();
            return false;
        }

        return true;
    }

    inline bool Mmap::is_open() const { return addr_ != MAP_FAILED; }

    inline size_t Mmap::size() const { return size_; }

    inline const char *Mmap::data() const { return (const char *)addr_; }

    inline void Mmap::close()
    {
#if defined(_WIN32)
        if (addr_)
        {
            ::UnmapViewOfFile(addr_);
            addr_ = MAP_FAILED;
        }

        if (hMapping_)
        {
            ::CloseHandle(hMapping_);
            hMapping_ = NULL;
        }

        if (hFile_ != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(hFile_);
            hFile_ = INVALID_HANDLE_VALUE;
        }
#else
        if (addr_ != MAP_FAILED)
        {
            munmap(addr_, size_);
            addr_ = MAP_FAILED;
        }

        if (fd_ != -1)
        {
            ::close(fd_);
            fd_ = -1;
        }
#endif
        size_ = 0;
    }
}
