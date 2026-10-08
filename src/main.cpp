
#include "AudioDeviceManager.hpp"
#include "AudioAsyncDevice.hpp"
#include "AudioSyncDevice.hpp"
#include "AutoSpeechRecognition.hpp"
#include "Device.hpp"
#include "common-whisper.h"
#include "TextToSpeech.hpp"
#include "language_synthesizer/LanguageSynthesizer.hpp"

#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <bit>
#include <fstream>
#include <ggml-backend.h>
#include <whisper.h>
#include <cstdlib>
#include <iostream>

static constexpr const char* kBackendPath = "../external/bins";

int main ([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    ggml_backend_load_all_from_path(kBackendPath);
    Ark::ULanguageSynthesizer Synthesizer("../models/Qwen3.5-0.8B-Q4_0.gguf");

    auto Response {
        Synthesizer.Generate("The day after monday is: ", 32)
    };


    if (Response)
    {
        std::cout << "Response: " << *Response << std::endl;
    } else 
    {
        std::cout << "Failed to generate Repsonse\n";
    }
    return EXIT_SUCCESS;
}
