#
# author: Adam Budziak
#

file(GLOB_RECURSE GRAFPE_SOURCES "src/*.cpp")
file(GLOB_RECURSE GRAFPE_HEADERS "include/grafpe/*.hpp")

set(GRAFPE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(GraFPE ${GRAFPE_SOURCES} ${GRAFPE_HEADERS})
target_include_directories(GraFPE PUBLIC ${GRAFPE_INCLUDE_DIR})

find_package(Boost)

if (Boost_FOUND)
    message("Boost FOUND!")
    option(USE_BOOST
            "Boost is available, so use it whenever needed" ON)
elseif()
    message("Boost not found.")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/GrafpeConfig.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/GrafpeConfig.hpp"
)

target_include_directories(GraFPE PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if (UNIX)
    target_link_libraries(GraFPE ssl crypto DFARE pur pthread)
elseif(WIN32)
#    file(TO_CMAKE_PATH $ENV{OPENSSL_ROOT} OPENSSL_ROOT_DIR)
    file(TO_CMAKE_PATH "C:\\\\Program Files\\OpenSSL" OPENSSL_ROOT_DIR)
    message(INFO: " root: ${OPENSSL_ROOT_DIR}")
    find_package(OpenSSL)

    message(INFO: " OpenSSL_VERSION ${OPENSSL_VERSION}")
    message(INFO: " OpenSSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR}")
    message(INFO: " OpenSSL_LIBRARIES ${OPENSSL_LIBRARIES}")

    target_include_directories(GraFPE PUBLIC ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(GraFPE ${OPENSSL_LIBRARIES} DFARE pur )
endif()
