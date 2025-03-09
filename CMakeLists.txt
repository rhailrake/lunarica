cmake_minimum_required(VERSION 3.14)
project(lunarica
        VERSION 0.1.0
        LANGUAGES CXX
        DESCRIPTION "lunarica http client"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(BUILD_TESTS "Build tests" ON)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
else()
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

set(REPLXX_VERSION "master")
set(JSONCPP_VERSION "1.9.5")
set(HTTPLIB_VERSION "v0.12.0")
set(FMT_VERSION "9.1.0")
set(CPP_BASE64_VERSION "master")
set(UTF8CPP_VERSION "v3.2.3")
set(GTEST_VERSION "release-1.12.1")

FetchContent_Declare(
        replxx
        GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
        GIT_TAG ${REPLXX_VERSION}
)

set(JSONCPP_WITH_TESTS OFF CACHE BOOL "")
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "")
set(JSONCPP_WITH_CMAKE_PACKAGE OFF CACHE BOOL "")
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(BUILD_STATIC_LIBS ON CACHE BOOL "")

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG ${JSONCPP_VERSION}
)

FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG ${HTTPLIB_VERSION}
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG ${FMT_VERSION}
)

FetchContent_Declare(
        cpp_base64
        GIT_REPOSITORY https://github.com/ReneNyffenegger/cpp-base64.git
        GIT_TAG ${CPP_BASE64_VERSION}
)

FetchContent_Declare(
        utf8cpp
        GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
        GIT_TAG ${UTF8CPP_VERSION}
)

FetchContent_MakeAvailable(replxx jsoncpp httplib fmt utf8cpp)

FetchContent_MakeAvailable(cpp_base64)
if(NOT TARGET cpp_base64)
    add_library(cpp_base64 STATIC
            ${cpp_base64_SOURCE_DIR}/base64.cpp
    )
    target_include_directories(cpp_base64 PUBLIC
            ${cpp_base64_SOURCE_DIR}
    )
endif()

add_library(lunarica_lib STATIC ${SOURCES})
add_executable(lunarica src/main.cpp)

target_link_libraries(lunarica_lib PUBLIC
        replxx::replxx
        jsoncpp_static
        httplib::httplib
        fmt::fmt
        cpp_base64
)

target_link_libraries(lunarica PRIVATE lunarica_lib)

if(WIN32)
    target_link_libraries(lunarica_lib PUBLIC ws2_32)
elseif(UNIX)
    target_link_libraries(lunarica_lib PUBLIC pthread dl)
    target_link_libraries(lunarica_lib PUBLIC ssl crypto)
endif()

target_include_directories(lunarica_lib PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${jsoncpp_SOURCE_DIR}/include"
        "${utf8cpp_SOURCE_DIR}/source"
)

if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG ${GTEST_VERSION}
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
    )

    add_executable(lunarica_tests ${TEST_SOURCES})
    target_link_libraries(lunarica_tests PRIVATE
            lunarica_lib
            gtest
            gtest_main
            gmock
    )

    target_include_directories(lunarica_tests PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src"
            "${gtest_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include"
    )

    include(GoogleTest)
    gtest_discover_tests(lunarica_tests)
endif()