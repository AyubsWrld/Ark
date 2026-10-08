add_library(arklangsynth
        LanguageSynthesizer.cpp
)

find_package(llama REQUIRED)
find_package(spdlog REQUIRED)

target_include_directories(arklangsynth
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include/language_synthesizer"
)

target_link_libraries(arklangsynth
        PRIVATE
        llama
        spdlog::spdlog
)