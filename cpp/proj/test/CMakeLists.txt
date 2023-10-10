#
# author: Adam Budziak
#

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)

add_library(CatchLib STATIC ${CATCH_INCLUDE_DIR}/test_main)

function(addCatchTest target)
    add_executable(${target} ${ARGN})
    add_test(${target} ${target})

    target_link_libraries(${target} GraFPE DFARE pur CatchLib)
endfunction()

file(GLOB_RECURSE PURLIB_TEST_FILES "purlib/*.cpp")
file(GLOB_RECURSE GRAFPE_TEST_FILES "grafpe/*.cpp")
file(GLOB_RECURSE DFARE_TEST_FILES "dfare/*.cpp")

addCatchTest(AllTests
        ${PURLIB_TEST_FILES}
        ${GRAFPE_TEST_FILES}
        ${DFARE_TEST_FILES}
        )

add_subdirectory(purlib)
add_subdirectory(grafpe)
add_subdirectory(dfare)

