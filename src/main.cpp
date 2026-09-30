
/*
 *  At the highest level, the program starts by configuring itself, opens the
 * microphone, loads a Whisper model, and then enters a loop. During every
 * iteration of that loop, it collects some recently recorded audio, prepares
 * that audio into a form Whisper can consume, runs Whisper inference, extracts
 * the resulting text, prints it, and then repeats.
 */
#include "RingBuffer.hpp"
#include "whisper.h"

#include "AudioDeviceManager.hpp"
#include "AudioAsyncDevice.hpp"
#include "AudioSyncDevice.hpp"
#include "AudioVisualizer.hpp"
#include "common-whisper.h"

#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <cstdlib>
#include <iostream>


using RingBuffer = Ark::TRingBuffer<float, 8192>;

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
    		break;
    	}

    }
    return EXIT_SUCCESS;
}
