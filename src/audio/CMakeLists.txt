add_library(arkaudio)

target_sources(arkaudio
    PUBLIC
    sdl_impl.cpp
)

target_include_directories(arkaudio
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/audio"
)
