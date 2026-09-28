
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
#include "AudioVisualizer.hpp"

#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <SDL_stdinc.h>
#include <cstdlib>
#include <iostream>


using RingBuffer = Ark::TRingBuffer<float, 8192>;

int main (int argc, char **argv)
{
    using namespace Ark;
    UAudioAsyncDevice<float> Device; 
    Device.Init();
    Device.Play();
    for(;;)
    {
    }
    return EXIT_SUCCESS;
}
