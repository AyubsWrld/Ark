#pragma once

#include <thread>
#include <cstdint>
#include "whisper.h"

namespace Ark
{
    using FWhisperContextParameters = whisper_context_params ;
    using FWhisperContext = whisper_context;

    struct FWhisperParameters
    {
        int32_t n_threads  = std::min(4, (int32_t) std::thread::hardware_concurrency());
        int32_t step_ms    = 3000;
        int32_t length_ms  = 10000;
        int32_t keep_ms    = 200;
        int32_t capture_id = -1;
        int32_t max_tokens = 32;
        int32_t audio_ctx  = 0;
        int32_t beam_size  = -1;

        float vad_thold    = 0.6f;
        float freq_thold   = 100.0f;

        bool translate     = false;
        bool no_fallback   = false;
        bool print_special = false;
        bool no_context    = true;
        bool no_timestamps = false;
        bool tinydiarize   = false;
        bool save_audio    = false; // save audio to wav file
        bool use_gpu       = true; // Why does setting this to false cause segfault?
        bool flash_attn    = true;

        std::string language  = "en";
        std::string model     = "models/ggml-base.en.bin";
        std::string fname_out;

    };

    std::ostream& operator<<(std::ostream& o, const FWhisperParameters& params);
    void PrintUsage(int argc, char ** argv, const FWhisperParameters& params);
    [[nodiscard]] FWhisperParameters ParseWhisperParameters(int argc, char ** argv);
    int ASREntry(int argc, char ** argv);
}
