
/*
 *  At the highest level, the program starts by configuring itself, opens the microphone,
 *  loads a Whisper model, and then enters a loop. During every iteration of that loop, it
 *  collects some recently recorded audio, prepares that audio into a form Whisper can consume,
 *  runs Whisper inference, extracts the resulting text, prints it, and then repeats.
 */
#include "common-sdl.h"
#include "common.h"
#include "common-whisper.h"
#include "whisper.h"
#include "asr.hpp"

#include <cstdio>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <print>

int main(int argc, char ** argv) 
{
    Ark::ASREntry(argc, argv);
    return EXIT_SUCCESS;
}
