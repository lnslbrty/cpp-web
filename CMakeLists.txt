cmake_minimum_required(VERSION 2.8.9)
project(cpp-web)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release ... FORCE)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -fno-pic -fomit-frame-pointer -flto -fno-rtti -ffunction-sections -fdata-sections")

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
if(CMAKE_BUILD_TYPE MATCHES Release)
    set_target_properties(cpp-web PROPERTIES LINK_FLAGS "-no-pie -flto -Wl,--gc-sections -Wl,-z,norelro -Wl,--hash-style=gnu -Wl,--build-id=none")
endif()
target_link_libraries(cpp-web pthread)
