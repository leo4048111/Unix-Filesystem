#include "Defines.hpp"

namespace ufs
{
    class FileManager
    {
        SINGLETON(FileManager)

    public:
        Error mount();
        Error unmount();

        bool isMounted() const { return _isMounted; }

    private:
        bool _isMounted{false};
    };
}