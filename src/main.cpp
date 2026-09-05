
/*
 *  At the highest level, the program starts by configuring itself, opens the
 * microphone, loads a Whisper model, and then enters a loop. During every
 * iteration of that loop, it collects some recently recorded audio, prepares
 * that audio into a form Whisper can consume, runs Whisper inference, extracts
 * the resulting text, prints it, and then repeats.
 */
#include "asr.hpp"
#include "whisper.h"

#include "AudioDeviceManager.hpp"
#include "AudioAsyncDevice.hpp"
#include "AudioVisualizer.hpp"

#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <SDL_stdinc.h>
#include <cstdlib>
#include <iostream>

enum
{
  CAPTURE_TIME_MS = 3000
};


static FAudioDevice dev; 
static SDL_AudioDeviceID m_id;


void Resume()
{
  SDL_PauseAudioDevice(m_id, 0);
  SDL_Delay(CAPTURE_TIME_MS);
  SDL_PauseAudioDevice(m_id, 1);
}

bool SDL_setup(int capture_id, int sample_rate)
{

  // Set the priority of a particular log category.
  SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
      return false;
  }

  SDL_SetHintWithPriority(SDL_HINT_AUDIO_RESAMPLING_MODE, "medium", SDL_HINT_OVERRIDE);

  {
      int nDevices = SDL_GetNumAudioDevices(SDL_TRUE);
      fprintf(stderr, "%s: found %d capture devices:\n", __func__, nDevices);
      for (int i = 0; i < nDevices; i++) {
          fprintf(stderr, "%s:    - Capture device #%d: '%s'\n", __func__, i, SDL_GetAudioDeviceName(i, SDL_TRUE));
      }
  }


  SDL_AudioSpec capture_spec_requested;
  SDL_AudioSpec capture_spec_obtained;

  SDL_zero(capture_spec_requested);
  SDL_zero(capture_spec_obtained);

  capture_spec_requested.freq     = sample_rate;
  capture_spec_requested.format   = AUDIO_F32;
  capture_spec_requested.channels = 1;
  capture_spec_requested.samples  = 1024;
  capture_spec_requested.callback = [](void * userdata, uint8_t * stream, int len) {
      FAudioDevice* audio = (FAudioDevice*) userdata;
      audio->cb(stream, len);
  };
  capture_spec_requested.userdata = &dev;

  if (capture_id >= 0) {
      fprintf(stderr, "%s: attempt to open capture device %d : '%s' ...\n", __func__, capture_id, SDL_GetAudioDeviceName(capture_id, SDL_TRUE));
      m_id = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(capture_id, SDL_TRUE), SDL_TRUE, &capture_spec_requested, &capture_spec_obtained, 0);
  } else {
      fprintf(stderr, "%s: attempt to open default capture device ...\n", __func__);
      m_id = SDL_OpenAudioDevice(nullptr, SDL_TRUE, &capture_spec_requested, &capture_spec_obtained, 0);
  }

  if (!m_id) {
      fprintf(stderr, "%s: couldn't open an audio device for capture: %s!\n", __func__, SDL_GetError());
      m_id = 0;

      return false;
  } else {
      fprintf(stderr, "%s: obtained spec for input device (SDL Id = %d):\n", __func__, m_id);
      fprintf(stderr, "%s:     - sample rate:       %d\n",                   __func__, capture_spec_obtained.freq);
      fprintf(stderr, "%s:     - format:            %d (required: %d)\n",    __func__, capture_spec_obtained.format,
              capture_spec_requested.format);
      fprintf(stderr, "%s:     - channels:          %d (required: %d)\n",    __func__, capture_spec_obtained.channels,
              capture_spec_requested.channels);
      fprintf(stderr, "%s:     - samples per frame: %d\n",                   __func__, capture_spec_obtained.samples);
  }

  /*
  m_sample_rate = capture_spec_obtained.freq;
  m_audio.resize((m_sample_rate*m_len_ms)/1000);
  */
  return true;
}

// enum class byte : unsigned char {};

void callback (std::byte *stream, int len) 
{
  std::cout << "Captured " << len << " samples";
}

class Graph 
{
 public:

    std::vector<int> operator()(int width, int height) const 
    {
        std::vector<int> output(width);
        for (int i = 0; i < width; ++i) 
        {
            float v = 0;
            v += 0.1f * sin((i + shift) * 0.1f);        // NOLINT
            v += 0.2f * sin((i + shift + 10) * 0.15f);  // NOLINT
            v += 0.1f * sin((i + shift) * 0.03f);       // NOLINT
            v *= height;                                // NOLINT
            v += 0.5f * height;                         // NOLINT
            output[i] = static_cast<int>(v);
        }
        return output;
    }
    int shift = 0;
};

std::vector<int> triangle(int width, int height) {
  std::vector<int> output(width);
  for (int i = 0; i < width; ++i) {
    output[i] = i % (height - 4) + 2;
  }
  return output;
}

int main (int argc, char **argv)
{
    // Stub to remove, only for prototyping ...
    //Ark::FAudioVisualizer<void>{}.Draw();
  
    using namespace Ark;
    //UAudioDeviceManager<UAudioAsyncDevice<float>> Manager;
    UAudioAsyncDevice<float> Device; 
    Device.Init();
    Device.Play();
    for(;;)
    {
    }
    return EXIT_FAILURE;
}
