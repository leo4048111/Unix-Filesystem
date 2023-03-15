#include "Defines.hpp"

#include "SuperBlock.hpp"

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

        void loadSuperBlock(SuperBlock& superblock);
    private:
        bool _isMounted{false};

        unsigned int _fsStat { FS_UNINITIALIZED };
    };
}