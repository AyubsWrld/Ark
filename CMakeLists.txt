cmake_minimum_required(VERSION 3.20) 
project(
    edith
    LANGUAGES C CXX
    VERSION 1.0
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Point this at the root of your whisper.cpp SDK folder
# (the one containing include/, lib/, and bin/)
set(WHISPER_SDK_DIR "${CMAKE_SOURCE_DIR}/third_party/whisper-sdk" CACHE PATH "Path to whisper.cpp SDK")

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    message(STATUS "Found SDL2")
endif()

# --- whisper.cpp / ggml imported targets ---
find_path(WHISPER_INCLUDE_DIR
    NAMES whisper.h
    PATHS ${WHISPER_SDK_DIR}/include
    REQUIRED
)

find_library(WHISPER_LIBRARY
    NAMES whisper libwhisper
    PATHS ${WHISPER_SDK_DIR}/lib ${WHISPER_SDK_DIR}/bin
    REQUIRED
)

find_library(GGML_LIBRARY
    NAMES ggml
    PATHS ${WHISPER_SDK_DIR}/lib ${WHISPER_SDK_DIR}/bin
    REQUIRED
)

add_library(whisper SHARED IMPORTED)
set_target_properties(whisper PROPERTIES
    IMPORTED_LOCATION "${WHISPER_SDK_DIR}/bin/whisper.dll"
    IMPORTED_IMPLIB "${WHISPER_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${WHISPER_INCLUDE_DIR}"
)

add_library(ggml SHARED IMPORTED)
set_target_properties(ggml PROPERTIES
    IMPORTED_LOCATION "${WHISPER_SDK_DIR}/bin/ggml.dll"
    IMPORTED_IMPLIB "${GGML_LIBRARY}"
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIR}
    ${WHISPER_INCLUDE_DIR}
)

target_sources(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    src/common.cpp
    src/common-sdl.cpp
    src/common-whisper.cpp
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${SDL2_LIBRARIES}
    whisper
    ggml
)

# Copy required DLLs next to the exe after build (Windows needs them at runtime)
if (WIN32)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${WHISPER_SDK_DIR}/bin/whisper.dll"
            "${WHISPER_SDK_DIR}/bin/ggml.dll"
            "${WHISPER_SDK_DIR}/bin/ggml-base.dll"
            #"${WHISPER_SDK_DIR}/bin/ggml-cpu.dll"
            $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
        COMMENT "Copying whisper/ggml runtime DLLs"
    )
endif()
