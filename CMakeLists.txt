
cmake_minimum_required(VERSION 4.3)
#set(CMAKE_EXPERIMENTAL_GENERATE_SBOM ON)
set(
    CMAKE_EXPERIMENTAL_GENERATE_SBOM
    "ca494ed3-b261-4205-a01f-603c95e4cae0"
)

project(
    edith
    LANGUAGES C CXX
    VERSION 1.0
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    WHISPER_SDK_DIR
    "${CMAKE_SOURCE_DIR}/third_party/whisper-sdk"
    CACHE PATH
    "Path to whisper.cpp SDK"
)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
    PUBLIC

    src/main.cpp
    src/common-whisper.cpp
    src/common.cpp
    src/common-sdl.cpp
)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/asr)


find_package(SDL2 REQUIRED)

find_path(
    WHISPER_INCLUDE_DIR
    NAMES whisper.h
    PATHS "${WHISPER_SDK_DIR}/include"
    REQUIRED
)

find_library(
    WHISPER_LIBRARY
    NAMES whisper libwhisper
    PATHS
        "${WHISPER_SDK_DIR}/lib"
        "${WHISPER_SDK_DIR}/bin"
    REQUIRED
)

find_library(
    GGML_LIBRARY
    NAMES ggml
    PATHS
        "${WHISPER_SDK_DIR}/lib"
        "${WHISPER_SDK_DIR}/bin"
    REQUIRED
)

# -------------------------------------------------------
# Represent prebuilt dependencies as CMake targets
# -------------------------------------------------------

# add_library(whisper SHARED IMPORTED)
#
# set_target_properties(
#     whisper
#     PROPERTIES
#
#     IMPORTED_LOCATION
#         "${WHISPER_SDK_DIR}/bin/whisper.dll"
#
#     IMPORTED_IMPLIB
#         "${WHISPER_LIBRARY}"
#
#     INTERFACE_INCLUDE_DIRECTORIES
#         "${WHISPER_INCLUDE_DIR}"
#
#     SPDX_LICENSE
#         "MIT"
# )
#
#
# add_library(ggml SHARED IMPORTED)

# -------------------------------------------------------
# edith
# -------------------------------------------------------

target_include_directories(
    edith
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/asr"
        ${SDL2_INCLUDE_DIR}
        ${WHISPER_INCLUDE_DIR}
)

target_link_libraries(
    edith
    PRIVATE
        ${SDL2_LIBRARIES}
        whisper
        ggml
        edasr
)

