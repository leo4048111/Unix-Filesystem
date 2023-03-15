#include "Defines.hpp"

namespace ufs
{
    class FileSystem
    {
        SINGLETON(FileSystem)

    private:
        enum FS_STATUS
        {
            FS_UNINITIALIZED, // uninitialized
            FS_NOFORM,        // initialized but not formatted
            FS_READY          // initialized and formatted
        };

    public:
        Error mount();
        Error unmount();

        bool isMounted() const { return _isMounted; }

    private:
        bool _isMounted{false};

        unsigned int _fsStat { FS_UNINITIALIZED };
    };
}