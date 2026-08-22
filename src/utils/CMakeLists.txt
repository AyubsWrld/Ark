project(arkutils)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PUBLIC 
  PlatformMisc.cpp
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC 
  "${CMAKE_SOURCE_DIR}/include/utils"
)


