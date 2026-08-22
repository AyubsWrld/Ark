find_package(SDL2 REQUIRED)

add_library(arkcommon
  STATIC
  common-sdl.cpp
  common-whisper.cpp
  common.cpp
  stb_vorbis.c
)

target_include_directories(arkcommon
  PUBLIC
  "${CMAKE_SOURCE_DIR}/include/common"
  ${SDL2_INCLUDE_DIRS}
)
