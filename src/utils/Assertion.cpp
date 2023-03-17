#include "SuperBlock.hpp"
#include "DiskBlock.hpp"
#include "Defines.hpp"
#include "DiskInode.hpp"
#include "InodeTable.hpp"

template <typename T, int s, bool x = sizeof(T) == s>
class CheckSize;

template <typename T, int s>
class CheckSize<T, s, true> {
public:
  static constexpr bool result() {
    return true;
  }
};

template <typename T, int s>
class CheckSize<T, s, false> {
public:
  static constexpr bool result() {
    return false;
  }
};

static_assert(CheckSize<ufs::InodeTable, 3 * DISK_BLOCK_SIZE>::result(), "SuperBlock size is not equal to DiskBlock size");
static_assert(CheckSize<ufs::SuperBlock, DISK_BLOCK_SIZE>::result(), "SuperBlock size is not equal to DiskBlock size");
static_assert(CheckSize<ufs::DiskBlock, DISK_BLOCK_SIZE>::result(), "DiskBlock size is not equal to DiskBlock size");
static_assert(CheckSize<ufs::DiskInode, DISKINODE_SIZE>::result(), "Inode size is not equal to DISKINODE_SIZE");