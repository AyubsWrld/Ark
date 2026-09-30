add_library(arkasr)

target_sources(arkasr
    PUBLIC
    AutoSpeechRecognition.cpp
)

find_package(SDL2 REQUIRED)
find_package(spdlog REQUIRED)

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
        spdlog::spdlog
        #${SDL2_LIBRARIES}
)
