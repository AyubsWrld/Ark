cmake_minimum_required(VERSION 3.16)

project(
    ark
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

find_package(whisper REQUIRED)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
    PUBLIC
    src/main.cpp
    src/common-whisper.cpp
    src/common.cpp
    src/common-sdl.cpp
)

#add_subdirectory(${CMAKE_SOURCE_DIR}/src/common) # this behaves weirdly on darwin. 
add_subdirectory(${CMAKE_SOURCE_DIR}/src/asr)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/utils)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/audio)


find_package(SDL2 REQUIRED)
find_package(GGML REQUIRED)

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

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/asr"
        "${CMAKE_SOURCE_DIR}/include/SDLWrapper"
        ${SDL2_INCLUDE_DIR}
        ${WHISPER_INCLUDE_DIR}
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
        ${SDL2_LIBRARIES}
        ${WHISPER_LIBRARIES}
        ${GGML_LIBRARIES}
        whisper
        arkasr
        arkaudio
        arkutils
        #arkcommon
)


