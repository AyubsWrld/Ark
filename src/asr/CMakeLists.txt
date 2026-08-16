add_library(edasr) 

target_sources(edasr
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
    edasr
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/asr
        ${CMAKE_SOURCE_DIR}/include/
        ${SDL2_INCLUDE_DIR}
        ${WHISPER_INCLUDE_DIR}
)

target_link_libraries(
    edasr
    PRIVATE
        ${SDL2_LIBRARIES}
        # whisper
        # ggml
)
