#
# author: Adam Budziak
#

file(GLOB_RECURSE PURLIB_SOURCES "src/*.cpp")
file(GLOB_RECURSE PURLIB_HEADERS "include/pur/*.hpp")

set(PURLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(pur ${PURLIB_SOURCES} ${PURLIB_HEADERS})
target_include_directories(pur PUBLIC ${PURLIB_INCLUDE_DIR})

if(CLANG_TIDY_EXE)
    set_target_properties(
            pur PROPERTIES
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
endif()