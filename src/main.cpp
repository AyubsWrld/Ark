
/*
 *  At the highest level, the program starts by configuring itself, opens the
 * microphone, loads a Whisper model, and then enters a loop. During every
 * iteration of that loop, it collects some recently recorded audio, prepares
 * that audio into a form Whisper can consume, runs Whisper inference, extracts
 * the resulting text, prints it, and then repeats.
 */
#include "RingBuffer.hpp"
#include "asr.hpp"
#include "whisper.h"

#include "AudioDeviceManager.hpp"
#include "AudioAsyncDevice.hpp"
#include "AudioSyncDevice.hpp"
#include "AudioVisualizer.hpp"
#include "common-whisper.h"

#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <SDL_stdinc.h>
#include <cstdlib>
#include <iostream>
#include <llama.h>


using RingBuffer = Ark::TRingBuffer<float, 8192>;

std::string Talk(std::string Prompt)
{
	const char* ModelPath = "models/model.gguf";

    ggml_backend_load_all();

    llama_model_params ModelParams =
        llama_model_default_params();

    llama_model* Model =
        llama_model_load_from_file(
            ModelPath,
            ModelParams);

    if (!Model)
    {
        std::fprintf(stderr, "Failed to load model\n");
    	std::exit(EXIT_FAILURE);
    }

    const llama_vocab* Vocab =
        llama_model_get_vocab(Model);

    llama_context_params ContextParams =
        llama_context_default_params();

    llama_context* Context =
        llama_init_from_model(
            Model,
            ContextParams);

    if (!Context)
    {
        std::fprintf(stderr, "Failed to create context\n");
        llama_model_free(Model);
    	std::exit(EXIT_FAILURE);
    }

    llama_sampler* Sampler =
        llama_sampler_chain_init(
            llama_sampler_chain_default_params());

    llama_sampler_chain_add(
        Sampler,
        llama_sampler_init_temp(0.8f));

    llama_sampler_chain_add(
        Sampler,
        llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    const int TokenCount =
        -llama_tokenize(
            Vocab,
            Prompt.c_str(),
            Prompt.size(),
            nullptr,
            0,
            true,
            true);

    std::vector<llama_token> Tokens(TokenCount);

    llama_tokenize(
        Vocab,
        Prompt.c_str(),
        Prompt.size(),
        Tokens.data(),
        Tokens.size(),
        true,
        true);

    llama_batch Batch =
        llama_batch_get_one(
            Tokens.data(),
            Tokens.size());

    while (true)
    {
        if (llama_decode(Context, Batch) != 0)
        {
            break;
        }

        llama_token Token =
            llama_sampler_sample(
                Sampler,
                Context,
                -1);

        if (llama_vocab_is_eog(Vocab, Token))
        {
            break;
        }

        char Buffer[256];

        const int Length =
            llama_token_to_piece(
                Vocab,
                Token,
                Buffer,
                sizeof(Buffer),
                0,
                true);

        if (Length > 0)
        {
            std::fwrite(
                Buffer,
                1,
                Length,
                stdout);
        }

        Batch =
            llama_batch_get_one(
                &Token,
                1);
    }

    std::printf("\n");

    llama_sampler_free(Sampler);
    llama_free(Context);
    llama_model_free(Model);

    return {};
}

int main (int argc, char **argv)
{
	ggml_backend_load_all();
    using namespace Ark;
    UAudioSyncDevice Device;
    Device.Init();
    Device.Play();
    for(;;)
    {
    	if (Device.GetInfo().State != EAudioDeviceState::Paused)
    	{
    		spdlog::info("Capturing Audio");
    	}else
    	{
    		spdlog::warn("Finished Capturing Audio");
    		const char* ModelPath = "../models/ggml-base.en.bin";
    		const char* WavPath = "test_audio.wav";

    		// Load model.
    		whisper_context_params ContextParams =
				whisper_context_default_params();

    		whisper_context* Context =
				whisper_init_from_file_with_params(
					ModelPath,
					ContextParams);

    		if (!Context)
    		{
    			std::fprintf(stderr, "Failed to load Whisper model\n");
    			return 1;
    		}

    		// Load WAV -> mono 32-bit float PCM.
    		std::vector<float> Audio;
    		std::vector<std::vector<float>> StereoAudio;

    		if (!read_audio_data(
					WavPath,
					Audio,
					StereoAudio,
					false))
    		{
    			std::fprintf(stderr, "Failed to load WAV file\n");
    			whisper_free(Context);
    			return 1;
    		}

    		// Stock inference parameters.
    		whisper_full_params Params =
				whisper_full_default_params(
					WHISPER_SAMPLING_GREEDY);

    		Params.language = "en";

    		// Run inference.
    		if (whisper_full(
					Context,
					Params,
					Audio.data(),
					static_cast<int>(Audio.size())) != 0)
    		{
    			std::fprintf(stderr, "Whisper inference failed\n");
    			whisper_free(Context);
    			return 1;
    		}

    		const int SegmentCount =
				whisper_full_n_segments(Context);

    		for (int i = 0; i < SegmentCount; ++i)
    		{
    			const char* Text =
					whisper_full_get_segment_text(Context, i);

    			std::printf("%s", Text);
    		}

    		std::printf("\n");
    		whisper_free(Context);
    		Talk("How are you?");
    		break;
    	}

    }
    return EXIT_SUCCESS;
}
