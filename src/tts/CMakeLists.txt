if(NOT EXISTS "${PIPER_ROOT}/include/piper.h")
    message(FATAL_ERROR
            "libpiper not found at '${PIPER_ROOT}'. Build and install it first "
            "(cmake --install <build> --prefix <dir>) or pass -DPIPER_ROOT=<dir>.")
endif()

find_path(PIPER_INCLUDE_DIR piper.h
        HINTS "${PIPER_ROOT}/include"
        REQUIRED)

find_library(PIPER_LIBRARY
        NAMES piper libpiper
        HINTS "${PIPER_ROOT}" "${PIPER_ROOT}/lib" "${PIPER_ROOT}/bin"
        REQUIRED)

find_library(ONNXRUNTIME_LIBRARY
        NAMES onnxruntime libonnxruntime
        HINTS "${PIPER_ROOT}/lib" "${PIPER_ROOT}" "${PIPER_ROOT}/bin"
        REQUIRED)

# PiperTTS is header-only, so this is an INTERFACE library.
add_library(arktts INTERFACE)

target_include_directories(arktts INTERFACE
        "${CMAKE_SOURCE_DIR}/include/tts"   # include/tts/PiperTTS.hpp
        "${PIPER_INCLUDE_DIR}")

target_link_libraries(arktts INTERFACE
        "${PIPER_LIBRARY}"
        "${ONNXRUNTIME_LIBRARY}")