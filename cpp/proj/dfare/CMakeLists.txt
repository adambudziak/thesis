#
# author: Adam Budziak
#

file(GLOB_RECURSE DFARE_SOURCES "src/*.cpp")
file(GLOB_RECURSE DFARE_HEADERS "include/dfare/*.hpp")

set(DFARE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(DFARE ${DFARE_SOURCES} ${DFARE_HEADERS})
target_include_directories(DFARE PUBLIC ${DFARE_INCLUDE_DIR})

target_link_libraries(DFARE pur)

if(CLANG_TIDY_EXE)
    set_target_properties(
            DFARE PROPERTIES
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
endif()
