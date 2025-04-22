cmake_minimum_required(VERSION 3.14)
project(install_zlib)

include(FetchContent)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG master
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/external_libraries/zlib" CACHE PATH "Install path prefix" FORCE)

FetchContent_MakeAvailable(zlib)

install(
    TARGETS zlibstatic
    EXPORT zlib-targets
    DESTINATION ${ZLIB_INSTALL_DIR}/lib
)
