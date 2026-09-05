#pragma once

#include "AudioDevice.hpp"

#include <cstdint>
#include <vector>

namespace Ark 
{
    template<typename T>
    class UAudioAsyncDevice 
    {
        // using AudioCallback = void(uint8_t * stream, int len);

        EAudioDeviceState   mState {EAudioDeviceState::Stopped} ;
        FAudioDeviceInfo    mDeviceInfo;
        std::uint32_t       mId; 
        std::uint32_t       SampleRate;
        std::vector<T>      mBuffer;

    public:


        UAudioAsyncDevice()     =   default;
        ~UAudioAsyncDevice()    =   default;

        UAudioAsyncDevice(UAudioAsyncDevice&) = delete;
        UAudioAsyncDevice(UAudioAsyncDevice&&);

        UAudioAsyncDevice& operator=(UAudioAsyncDevice&) = delete;
        UAudioAsyncDevice& operator=(UAudioAsyncDevice&&);

        std::span<T> Get() const noexcept;

        [[nodiscard]] EAudioDeviceError Clear()    noexcept;
        [[nodiscard]] EAudioDeviceError Init();
        [[nodiscard]] FAudioDeviceInfo  GetInfo() const noexcept;
        void Pause()    noexcept;
        void Play()   noexcept;
        void Callback(uint8_t * stream, int len) noexcept; 

    };
}
