#pragma once

#include "AudioDevice.hpp"
#include "RingBuffer.hpp"

#include <cstdint>
#include <vector>

namespace Ark 
{
    enum { BufferSize = 4 << 20 };

    template<typename T>
    class UAudioAsyncDevice 
    {
        // using AudioCallback = void(uint8_t * stream, int len);

        FAudioDeviceInfo		mDeviceInfo;
        TRingBuffer<T, 4096>	mBuffer; // 1kb ~ 8kb buffer.

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
