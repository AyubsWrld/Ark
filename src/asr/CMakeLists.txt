add_library(arkasr) 

target_sources(arkasr
    PUBLIC
    asr.cpp
)

find_package(SDL2 REQUIRED)

# find_library(
#     WHISPER_LIBRARY
#     NAMES whisper libwhisper
#     PATHS
#         "${WHISPER_SDK_DIR}/lib"
#         "${WHISPER_SDK_DIR}/bin"
#     REQUIRED
# )

# find_library(
#     GGML_LIBRARY
#     NAMES ggml
#     PATHS
#         "${WHISPER_SDK_DIR}/lib"
#         "${WHISPER_SDK_DIR}/bin"
#     REQUIRED
# )

target_include_directories(
    arkasr
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include/asr/"
        "${CMAKE_SOURCE_DIR}/include/audio/"
        "${CMAKE_SOURCE_DIR}/include/common/"
        ${SDL2_INCLUDE_DIRS}
        ${WHISPER_INCLUDE_DIR}
)

target_link_libraries(
    arkasr
    PRIVATE
        #${SDL2_LIBRARIES}
)
