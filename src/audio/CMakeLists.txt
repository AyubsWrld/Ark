find_package(SDL2 REQUIRED)

add_library(arkaudio)

find_package(spdlog REQUIRED)
find_package(ftxui REQUIRED)

target_sources(arkaudio
    PUBLIC
        sdl_impl.cpp
        AudioAsyncDevice.cpp
        AudioDeviceManager.cpp
        AudioDevice.cpp
        WavWriter.cpp
)

target_link_libraries(arkaudio
    PRIVATE 
    spdlog::spdlog
    ftxui::dom
    ftxui::screen
    ftxui::component
)
target_include_directories(arkaudio
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include/audio"
        "${CMAKE_SOURCE_DIR}/include/utils"
        ${SDL2_INCLUDE_DIRS}
)

