#include "AudioAsyncDevice.hpp"
#include "AudioDevice.hpp"
#include <SDL2/SDL_audio.h>
#include <SDL_stdinc.h>


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
        SDL_OpenAudioDevice(NULL, SDL_TRUE, )
        return EAudioDeviceError::Success;
    }

    template<typename T>
    FAudioDeviceInfo UAudioAsyncDevice<T>::GetInfo() const noexcept
    {
        return {};
    }
}
