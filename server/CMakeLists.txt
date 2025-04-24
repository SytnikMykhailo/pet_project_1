cmake_minimum_required(VERSION 3.14)
project(install_sqlite)

add_library(sqlite3 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/external_libraries/sqlite3.c)

target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/external_libraries)

set_target_properties(sqlite3 PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/external_libraries/lib
)