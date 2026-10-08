cmake_minimum_required(VERSION 3.18)

project(
        ark
        LANGUAGES C CXX
        VERSION 1.0
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SERVER OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_APP OFF CACHE BOOL "" FORCE)

set(PIPER_ROOT "${CMAKE_SOURCE_DIR}/external/piper/libpiper/install"
        CACHE PATH "libpiper install prefix (contains include/, lib/, share/espeak-ng-data)")

find_package(spdlog REQUIRED)
find_package(ftxui REQUIRED)
find_package(Tracy REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(whisper CONFIG REQUIRED)
find_package(GGML REQUIRED)

add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME}
        PUBLIC
        src/main.cpp
        src/common-whisper.cpp
        src/common.cpp
        src/common-sdl.cpp
)

add_subdirectory("${CMAKE_SOURCE_DIR}/src/asr")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/utils")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/audio")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/language_synthesizer")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/tts")
#add_subdirectory(${CMAKE_SOURCE_DIR}/external/llama-cpp)

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
        ${spdlog_INCLUDE_DIRS}
)

target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        PRIVATE
        ${SDL2_LIBRARIES}
        ${WHISPER_LIBRARIES}
        ${GGML_LIBRARIES}
        ftxui::screen
        ftxui::dom
        ftxui::component
        spdlog::spdlog
        whisper
        arkasr
        arkaudio
        arklangsynth
        arkutils
        arktts
)

# ---------------------------------------------------------------------------
# Piper runtime files: espeak-ng-data + DLLs next to the executable
# ---------------------------------------------------------------------------
set(PIPER_ESPEAK_DATA "")
foreach(candidate
        "${PIPER_ROOT}/espeak-ng-data"
        "${PIPER_ROOT}/share/espeak-ng-data")
    if(EXISTS "${candidate}")
        set(PIPER_ESPEAK_DATA "${candidate}")
        break()
    endif()
endforeach()

if(PIPER_ESPEAK_DATA)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PIPER_ESPEAK_DATA}"
            "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/espeak-ng-data")
else()
    message(WARNING
            "espeak-ng-data not found under ${PIPER_ROOT} (checked ./ and share/). "
            "Piper will not work until you copy it there.")
endif()

if(WIN32)
    file(GLOB PIPER_RUNTIME_DLLS
            "${PIPER_ROOT}/*.dll"
            "${PIPER_ROOT}/bin/*.dll"
            "${PIPER_ROOT}/lib/*.dll")
    foreach(dll IN LISTS PIPER_RUNTIME_DLLS)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll}" "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>")
    endforeach()
endif()