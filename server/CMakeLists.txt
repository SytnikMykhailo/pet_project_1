cmake_minimum_required(VERSION 3.14)
project(install_sqlite)

include(FetchContent)

FetchContent_Declare(
    sqlite
    GIT_REPOSITORY https://github.com/sqlite/sqlite.git
    GIT_TAG origin/trunk  # або конкретний реліз, наприклад, version-3.45.1
)

# Установка в ту саму папку, як ти робив для zlib
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/external_libraries/sqlite" CACHE PATH "Install path prefix" FORCE)

# Опції компіляції SQLite
set(SQLITE_ENABLE_COLUMN_METADATA ON CACHE BOOL "" FORCE)
set(SQLITE_ENABLE_FTS5 ON CACHE BOOL "" FORCE)
set(SQLITE_ENABLE_JSON1 ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(sqlite)

install(
    TARGETS sqlite3
    EXPORT sqlite-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)