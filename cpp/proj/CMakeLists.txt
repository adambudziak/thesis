if (UNIX)
#   set(ASAN_FLAGS "-fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
#
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS} -fsanitize-address-use-after-scope")
#   set(CMAKE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} ${ASAN_FLAGS}")
endif()

if (UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic")
endif()

add_subdirectory(purlib)
add_subdirectory(dfare)
add_subdirectory(grafpe)
add_subdirectory(test)
add_subdirectory(benchmarks)
add_subdirectory(clients)
add_subdirectory(examples)

if(UNIX)
SET_TARGET_PROPERTIES(GraFPE PROPERTIES
        COMPILE_FLAGS "-O3"
#        COMPILE_FLAGS "-g"
        )

SET_TARGET_PROPERTIES(build_graph_bench PROPERTIES
        COMPILE_FLAGS "-O3"
#        COMPILE_FLAGS "-g"
        )

SET_TARGET_PROPERTIES(full_build_benchmark PROPERTIES
        COMPILE_FLAGS "-O3"
#        COMPILE_FLAGS "-g"
        )
SET_TARGET_PROPERTIES(perm_gen PROPERTIES
        COMPILE_FLAGS "-O3"
        #        COMPILE_FLAGS "-g"
        )
SET_TARGET_PROPERTIES(CCNCrypt PROPERTIES
        COMPILE_FLAGS "-O3"
        #        COMPILE_FLAGS "-g"
        )
endif()

set_property(TARGET GraFPE PROPERTY CXX_STANDARD 17)
set_property(TARGET GraFPE PROPERTY CXX_STANDARD_REQUIRED ON)