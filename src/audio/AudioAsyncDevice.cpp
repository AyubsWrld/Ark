#include "AudioAsyncDevice.hpp"
#include "AudioDevice.hpp"
#include <SDL2/SDL_audio.h>
#include <SDL_stdinc.h>
#include <spdlog/spdlog.h>


namespace Ark 
{
    template<typename T> 
    EAudioDeviceError UAudioAsyncDevice<T>::Clear() noexcept
    {
        return EAudioDeviceError::Success;
    }

    template<typename T> 
    EAudioDeviceError UAudioAsyncDevice<T>::Pause() noexcept
    {
        return EAudioDeviceError::Success;
    }

    template<typename T> 
    EAudioDeviceError UAudioAsyncDevice<T>::Init()
    {
        // an opened audio device starts out paused
        // callback runs in a different thread from the main thread. 
    
        SDL_AudioSpec desired, obtained; 

        SDL_zero(desired);
        SDL_zero(obtained);

        desired.freq        = 48000; 
        desired.format      = AUDIO_F32SYS;
        desired.samples     = 1024;
        desired.channels    = 1;

        int deviceID = SDL_OpenAudioDevice(NULL, SDL_TRUE, &desired, &obtained, 0); 
        if (!deviceID)
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
                SDL_GetAudioDeviceName(deviceID, SDL_TRUE));
        return EAudioDeviceError::Success;
    }

    template<typename T>
    FAudioDeviceInfo UAudioAsyncDevice<T>::GetInfo() const noexcept
    {
        return {};
    }
}
