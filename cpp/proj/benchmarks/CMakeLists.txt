#
# author: Adam Budziak
#

add_executable(build_graph_bench build_graph_bench.cpp)
add_executable(full_build_benchmark full_build_benchmark.cpp)
add_executable(perm_gen perm_gen.cpp)
add_executable(grafpe_encrypt grafpe_encrypt.cpp)
if(UNIX)
	target_link_libraries(build_graph_bench GraFPE pthread)
	target_link_libraries(full_build_benchmark GraFPE pthread)
	target_link_libraries(perm_gen GraFPE pthread)
	target_link_libraries(grafpe_encrypt GraFPE pthread)
elseif(WIN32)
	target_link_libraries(build_graph_bench GraFPE)
	target_link_libraries(full_build_benchmark GraFPE)
	target_link_libraries(perm_gen GraFPE)
	target_link_libraries(grafpe_encrypt GraFPE)
endif()