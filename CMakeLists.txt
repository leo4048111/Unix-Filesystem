cmake_minimum_required(VERSION 3.8)
project(UNIX_FS)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "-m32") 

include_directories("third-party/rang/include")
include_directories("third-party/nameof/include")
include_directories("src/include")


file(GLOB_RECURSE SRC_DIR "src/*.cpp")

add_executable(
    UNIX_FS
    ${SRC_DIR}
)