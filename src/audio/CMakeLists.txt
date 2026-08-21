find_package(SDL2 REQUIRED)

add_library(arkaudio)

target_sources(arkaudio
    PRIVATE
        sdl_impl.cpp
)

target_include_directories(arkaudio
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include/audio"
        ${SDL2_INCLUDE_DIRS}
)

