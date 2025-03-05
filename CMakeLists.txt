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

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
else()
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
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
set(NLOHMANN_JSON_VERSION "v3.11.2")
set(HTTPLIB_VERSION "v0.12.0")
set(FMT_VERSION "9.1.0")
set(CPP_BASE64_VERSION "master")

FetchContent_Declare(
        replxx
        GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
        GIT_TAG ${REPLXX_VERSION}
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG ${NLOHMANN_JSON_VERSION}
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

FetchContent_MakeAvailable(replxx nlohmann_json httplib fmt)

FetchContent_MakeAvailable(cpp_base64)
if(NOT TARGET cpp_base64)
    add_library(cpp_base64 STATIC
            ${cpp_base64_SOURCE_DIR}/base64.cpp
    )
    target_include_directories(cpp_base64 PUBLIC
            ${cpp_base64_SOURCE_DIR}
    )
endif()

add_executable(lunarica ${SOURCES})

target_link_libraries(lunarica PRIVATE
        replxx::replxx
        nlohmann_json::nlohmann_json
        httplib::httplib
        fmt::fmt
        cpp_base64
)

target_include_directories(lunarica PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)