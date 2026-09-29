#include "AudioAsyncDevice.hpp"
#include "AudioDevice.hpp"
#include "AudioDeviceManager.hpp"
#include "AudioVisualizer.hpp"
#include "signals.hpp"
#include "RingBuffer.hpp"
#include "WavWriter.hpp"

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
        spdlog::info("Attempting to play deviceID: {}\n", mDeviceInfo.ID);
        if (SDL_GetAudioDeviceStatus(mDeviceInfo.ID) == SDL_AUDIO_PAUSED) { return; }
        SDL_PauseAudioDevice(mDeviceInfo.ID, SDL_TRUE);
        mDeviceInfo.State = EAudioDeviceState::Paused;
    }

    template<typename T> 
    void UAudioAsyncDevice<T>::Play() noexcept
    { 
        if (SDL_GetAudioDeviceStatus(mDeviceInfo.ID) == SDL_AUDIO_PLAYING)
        {
            spdlog::info("Already Playing device", mDeviceInfo.ID);
            return; 
        }
        spdlog::info("Attempting to play deviceID: {}\n", mDeviceInfo.ID);
        SDL_PauseAudioDevice(mDeviceInfo.ID, SDL_FALSE);
        mDeviceInfo.State = EAudioDeviceState::Playing;
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

        mDeviceInfo.ID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, SDL_TRUE), SDL_TRUE, &capture_spec_desired, &capture_spec_obtained, 0);

        mDeviceInfo.Name        =  SDL_GetAudioDeviceName(0,SDL_TRUE);
        mDeviceInfo.Format      =  SDL_GetAudioDeviceName(0,SDL_TRUE); // Swap see SDL_audio.h:123 for names...
        mDeviceInfo.Frequency   =  capture_spec_obtained.freq;
        mDeviceInfo.Size        =  capture_spec_obtained.size;
        mDeviceInfo.Samples		=  capture_spec_obtained.samples;
        mDeviceInfo.Channels     =  capture_spec_obtained.channels;


        if (!mDeviceInfo.ID) // TODO : this is all quite backend specific.
        {
            spdlog::error(
                    "[{:s}]: Failed to open audio device: {}",
                    __PRETTY_FUNCTION__,
                    SDL_GetError());
            return EAudioDeviceError::OpenFailure;
        }
        return EAudioDeviceError::Success;
    }

    template<typename T>
    void UAudioAsyncDevice<T>::Callback(uint8_t* stream, int len) noexcept
    {
        mBuffer.WriteBuffer((T*)stream, len / sizeof(T));
    	auto WriteAvailable = mBuffer.WriteAvailable();
    	if ( WriteAvailable <= 0 )
    	{
    		auto ReadAvailable = mBuffer.ReadAvailable();
    		WavWriter writer;
    		writer.Open("test_audio.wav", 16000, 16, 1);
    		T Buffer[4096];
    		mBuffer.ReadBuffer(Buffer, 4096);
    		writer.Write(Buffer, ReadAvailable);
    		std::cout << "Done Writing: " << Buffer[0] << std::endl;
    	}
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
