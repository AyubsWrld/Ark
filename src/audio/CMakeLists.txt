find_package(SDL2 REQUIRED)

add_library(arkaudio)

find_package(spdlog REQUIRED)

target_sources(arkaudio
    PUBLIC
        sdl_impl.cpp
        AudioAsyncDevice.cpp
        AudioDeviceManager.cpp
        AudioDevice.cpp
)

target_link_libraries(arkaudio
    PRIVATE 
    spdlog::spdlog
)
target_include_directories(arkaudio
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include/audio"
        ${SDL2_INCLUDE_DIRS}
)

