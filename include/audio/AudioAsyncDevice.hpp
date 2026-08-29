#pragma once

#include "AudioDevice.hpp"

#include <cstdint>
#include <vector>

namespace Ark 
{
    template<typename T>
    class UAudioAsyncDevice 
    {
        std::uint32_t   mId; 
        std::uint32_t   SampleRate;
        std::vector<T>  mBuffer;

    public:

        UAudioAsyncDevice();
        ~UAudioAsyncDevice();

        UAudioAsyncDevice(UAudioAsyncDevice&) = delete;
        UAudioAsyncDevice(UAudioAsyncDevice&&);

        UAudioAsyncDevice& operator=(UAudioAsyncDevice&) = delete;
        UAudioAsyncDevice& operator=(UAudioAsyncDevice&&);

        std::span<T> Get() const noexcept;

        EAudioDeviceError Clear()    noexcept;
        EAudioDeviceError Pause()    noexcept;
        EAudioDeviceError Resume()   noexcept;
        EAudioDeviceError Init();
        FAudioDeviceInfo  GetInfo() const noexcept;

    };
}
