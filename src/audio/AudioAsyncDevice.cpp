#include "AudioAsyncDevice.hpp"
#include "AudioDevice.hpp"
#include "AudioDeviceManager.hpp"
#include "AudioVisualizer.hpp"
#include "signals.hpp"

#include <SDL_audio.h>
#include <SDL_stdinc.h>
#include <iostream>
#include <cstddef>
#include <print>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <thread>

namespace Ark 
{
    template<typename T> 
    EAudioDeviceError UAudioAsyncDevice<T>::Clear() noexcept
    {
        return EAudioDeviceError::Success;
    }

    template<typename T> 
    void UAudioAsyncDevice<T>::Pause() noexcept
    { 
        spdlog::info("Attempting to play deviceID: {}\n", mId);
        if (SDL_GetAudioDeviceStatus(mId) == SDL_AUDIO_PAUSED) { return; }
        SDL_PauseAudioDevice(mId, SDL_TRUE);
        mState = EAudioDeviceState::Paused;
    }

    template<typename T> 
    void UAudioAsyncDevice<T>::Play() noexcept
    { 
        if (SDL_GetAudioDeviceStatus(mId) == SDL_AUDIO_PLAYING) 
        {
            spdlog::info("Already Playing device", mId);
            return; 
        }
        spdlog::info("Attempting to play deviceID: {}\n", mId);
        SDL_PauseAudioDevice(mId, SDL_FALSE);
        mState = EAudioDeviceState::Playing;
    }

    template<typename T> 
    EAudioDeviceError UAudioAsyncDevice<T>::Init()
    {
        // an opened audio device starts out paused
        // callback runs in a different thread from the main thread. 
 
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            spdlog::error(
                    "[{:s}]: Failed to Initialize Audio Subsytem: {}",
                    __PRETTY_FUNCTION__,
                    SDL_GetError());
            return EAudioDeviceError::SubsystemInitializationFailure;
        }

        SDL_AudioSpec capture_spec_desired,
                      capture_spec_obtained; 

        SDL_zero(capture_spec_desired);
        SDL_zero(capture_spec_obtained);


        capture_spec_desired.freq        = 48000; 
        capture_spec_desired.format      = AUDIO_F32SYS;
        capture_spec_desired.samples     = 1024;
        capture_spec_desired.channels    = 1;
        capture_spec_desired.callback = [](void * userdata, uint8_t * stream, int len) {
            UAudioAsyncDevice* audio = (UAudioAsyncDevice*) userdata;
            audio->Callback(stream, len);
        };
        capture_spec_desired.userdata = this;

        spdlog::info("{}", SDL_GetAudioDeviceName(0,SDL_TRUE));
        // SDL_OpenAudio(), unlike this function, always acts on device ID 1. As such,  this function will never return 1.
        mId = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, SDL_TRUE), SDL_TRUE, &capture_spec_desired, &capture_spec_obtained, 0);

        mDeviceInfo.name        =  SDL_GetAudioDeviceName(0,SDL_TRUE);
        mDeviceInfo.format      =  SDL_GetAudioDeviceName(0,SDL_TRUE); // Swap see SDL_audio.h:123 for names... 
        mDeviceInfo.frequency   =  capture_spec_obtained.freq;
        mDeviceInfo.size        =  capture_spec_obtained.size;
        mDeviceInfo.samples     =  capture_spec_obtained.samples;
        mDeviceInfo.channels     =  capture_spec_obtained.channels;


        if (!mId)
        {
            spdlog::error(
                    "[{:s}]: Failed to open audio device: {}",
                    __PRETTY_FUNCTION__,
                    SDL_GetError());
            return EAudioDeviceError::OpenFailure;
        }

        spdlog::info(
                "[{:s}]: Successfully opened audio device \"{}\"",
                __PRETTY_FUNCTION__,
                mId);
        spdlog::info(
            "\n"
            "Capture Specification Obtained for \"{}\"\n"
            "----------------------------------------\n"
            "  {:<12} {}\n"
            "  {:<12} {}\n"
            "  {:<12} {}\n"
            "  {:<12} {}\n"
            "----------------------------------------",
            SDL_GetAudioDeviceName(0, SDL_TRUE),
            "Frequency:", capture_spec_obtained.freq,
            "Format:",    capture_spec_obtained.format,
            "Samples:",   capture_spec_obtained.samples,
            "Channels:",  capture_spec_obtained.channels
        ); 
        return EAudioDeviceError::Success;
    }

    template<typename T>
    void UAudioAsyncDevice<T>::Callback(uint8_t* stream, int len) noexcept
    {
    }


    template<typename T>
    FAudioDeviceInfo UAudioAsyncDevice<T>::GetInfo() const noexcept
    {
        return {};
    }


    template UAudioAsyncDevice<float>::UAudioAsyncDevice();
    template EAudioDeviceError UAudioAsyncDevice<float>::Init();
    template void UAudioAsyncDevice<float>::Callback(uint8_t * stream, int len) noexcept;
    template void UAudioAsyncDevice<float>::Play() noexcept;

}
