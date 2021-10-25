cmake_minimum_required(VERSION 3.1.9)
project(cpp-web)

set(CPACK_PACKAGE_CONTACT "toni@impl.cc")
set(CPACK_DEBIAN_PACKAGE_NAME "cpp-web")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)

include(CPack)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -fpic -fno-omit-frame-pointer -fno-rtti -ffunction-sections -fdata-sections -fsanitize=address -fsanitize=leak")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -fpic -fomit-frame-pointer -ffunction-sections -fdata-sections -flto")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections")

set(INJA_SRCDIR "${PROJECT_SOURCE_DIR}/deps/inja" CACHE STRING
                "Path to the inja source directory.")
if(NOT EXISTS "${INJA_SRCDIR}/single_include/inja/inja.hpp")
    message(FATAL_ERROR "inja missing")
endif()

set(MD4C_SRCDIR "${PROJECT_SOURCE_DIR}/deps/md4c/src" CACHE STRING
                "Path to the md4c source directory.")
if(NOT EXISTS "${MD4C_SRCDIR}/md4c-html.h")
    message(FATAL_ERROR "md4c missing")
endif()

include_directories(${CPP_HTTPLIB_SRCDIR} ${INJA_SRCDIR}/single_include
                                          ${INJA_SRCDIR}/third_party/include
                                          ${MD4C_SRCDIR})
set(MD4C_SOURCES "${MD4C_SRCDIR}/entity.c"
                 "${MD4C_SRCDIR}/md4c.c"
                 "${MD4C_SRCDIR}/md4c-html.c")

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(cpp-web ${MD4C_SOURCES} ${SOURCES})
#target_compile_definitions(cpp-web PUBLIC CPPHTTPLIB_THREAD_POOL_COUNT=4)
#target_compile_options(cpp-web PUBLIC "-std=c++14")
if(CMAKE_BUILD_TYPE MATCHES Release)
    set_target_properties(cpp-web PROPERTIES LINK_FLAGS "-no-pie -flto -Wl,--gc-sections -Wl,-z,norelro -Wl,--hash-style=gnu -Wl,--build-id=none")
endif()
target_link_libraries(cpp-web stdc++fs pthread event magic)

install(TARGETS cpp-web RUNTIME DESTINATION bin)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/wwwroot/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cpp-web/wwwroot)
