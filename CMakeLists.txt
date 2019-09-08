cmake_minimum_required(VERSION 2.8.9)
project(cpp-web)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -flto")

set(CPP_HTTPLIB_SRCDIR "${PROJECT_SOURCE_DIR}/deps/cpp-httplib" CACHE STRING "Path to the cpp-httplib source directory.")
if(NOT EXISTS "${CPP_HTTPLIB_SRCDIR}/httplib.h")
    message(FATAL_ERROR "cpp-httplib missing")
endif()

set(INJA_SRCDIR "${PROJECT_SOURCE_DIR}/deps/inja" CACHE STRING "Path to the inja source directory.")
if(NOT EXISTS "${INJA_SRCDIR}/single_include/inja/inja.hpp")
    message(FATAL_ERROR "inja missing")
endif()

include_directories(${CPP_HTTPLIB_SRCDIR} ${INJA_SRCDIR}/single_include ${INJA_SRCDIR}/third_party/include)
file(GLOB SOURCES "src/*.cpp")
add_executable(cpp-web ${SOURCES})
#target_compile_definitions(cpp-web PUBLIC CPPHTTPLIB_THREAD_POOL_COUNT=4)
target_link_libraries(cpp-web pthread)
