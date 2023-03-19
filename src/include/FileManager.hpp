#include "Defines.hpp"

namespace ufs
{
    class FileManager
    {
        SINGLETON(FileManager)

    public:
        Error mount();
        Error unmount();
        Error format();
        Error ls();

        bool isMounted() const { return _isMounted; }

    private:
        int _curDirInodeNo{0};
        bool _isMounted{false};
    };
}