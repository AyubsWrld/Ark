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

        [[nodiscard]] EAudioDeviceError Clear()    noexcept;
        [[nodiscard]] EAudioDeviceError EAudioDeviceError Pause()    noexcept;
        [[nodiscard]] EAudioDeviceError EAudioDeviceError Resume()   noexcept;
        [[nodiscard]] EAudioDeviceError EAudioDeviceError Init();
        [[nodiscard]] EAudioDeviceError FAudioDeviceInfo  GetInfo() const noexcept;

    };
}
