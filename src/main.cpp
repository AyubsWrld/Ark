#include "whisper.h"
#include "common-sdl.h"
#include "common.h"

#include <chrono>
#include <cstdio>
#include <csignal>
#include <string>
#include <thread>
#include <vector>
#include "ggml-backend.h"

int main(int argc, char** argv) {
    // ---- Config ----
    const std::string model_path = argc > 1 ? argv[1] : "../models/ggml-base.en.bin";
    const int sample_rate       = WHISPER_SAMPLE_RATE; // 16000
    const int step_ms           = 3000;   // how often we run inference
    const int length_ms         = 10000;  // audio window length fed to whisper
    const int keep_ms           = 200;    // overlap kept between windows
    const int n_samples_step    = (1e-3 * step_ms)   * sample_rate;
    const int n_samples_len     = (1e-3 * length_ms) * sample_rate;
    const int n_samples_keep    = (1e-3 * keep_ms)   * sample_rate;

    // ---- Load model ----
    struct whisper_context_params cparams = whisper_context_default_params();
    struct whisper_context* ctx = whisper_init_from_file_with_params(model_path.c_str(), cparams);
    if (!ctx) {
        fprintf(stderr, "error: failed to load model from '%s'\n", model_path.c_str());
        return 1;
    }

    std::raise(SIGABRT);

    // ---- Init mic capture ----
    audio_async audio(length_ms);
    if (!audio.init(-1, sample_rate)) {
        fprintf(stderr, "error: failed to initialize audio capture\n");
        return 1;
    }
    audio.resume();

    std::vector<float> pcmf32;       // current window fed to whisper
    std::vector<float> pcmf32_old;   // tail kept from previous window
    std::vector<float> pcmf32_new;   // freshly captured audio

    printf("Listening... speak into the mic (Ctrl+C to quit)\n\n");

    auto t_last = std::chrono::high_resolution_clock::now();

    while (true) {
        // wait until we have at least one step's worth of new audio
        while (true) {
            audio.get(step_ms, pcmf32_new);
            if ((int) pcmf32_new.size() > n_samples_step / 2) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // build the window: keep tail of previous + new audio
        const int n_samples_new = pcmf32_new.size();
        const int n_samples_take = std::min(
            (int) pcmf32_old.size(),
            std::max(0, n_samples_keep + n_samples_len - n_samples_new)
        );

        pcmf32.resize(n_samples_take + n_samples_new);
        for (int i = 0; i < n_samples_take; i++) {
            pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_take + i];
        }
        memcpy(pcmf32.data() + n_samples_take, pcmf32_new.data(), n_samples_new * sizeof(float));

        pcmf32_old = pcmf32;

        // ---- Run inference ----
        whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        wparams.print_progress   = false;
        wparams.print_special    = false;
        wparams.print_realtime   = false;
        wparams.print_timestamps = false;
        wparams.single_segment   = true;
        wparams.no_context       = true;
        wparams.language         = "en";
        wparams.n_threads        = std::min(4, (int) std::thread::hardware_concurrency());

        if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0) {
            fprintf(stderr, "error: whisper_full failed\n");
            break;
        }

        const int n_segments = whisper_full_n_segments(ctx);
        for (int i = 0; i < n_segments; i++) {
            const char* text = whisper_full_get_segment_text(ctx, i);
            printf("%s", text);
            fflush(stdout);
        }
    }

    audio.pause();
    whisper_free(ctx);
    return 0;
}
