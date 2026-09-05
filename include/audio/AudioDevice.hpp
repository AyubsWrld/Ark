#pragma once

#include <concepts>
#include <span>
#include <string>

namespace Ark 
{
    enum class EAudioDeviceError 
    {
        Success,
        OpenFailure,
        SubsystemInitializationFailure,
        _max
    }; 

    enum class EAudioDeviceState : std::uint8_t 
    {
        Stopped, 
        Paused,
        Playing,
        _max

    };
    struct FAudioDeviceInfo
    {
        std::string     name;           // Device Name
        std::string     format;         // Audio data format 
        std::size_t     frequency;      // DSP frequency -- samples per second 
        std::size_t     size;           // Audio buffer size in bytes (calculated)
        std::uint16_t   samples;        // Audio buffer size in sample FRAMES (total samples divided by channel count) */
        std::uint8_t    channels;       // Number of channels: 1 mono, 2 stereo */
    };

    template<typename T>
    concept AudioDevice = requires (T audio_device)
    {
        // Private members cannot be accessed by concepts... 
        // audio_device.mBuffer;
        // audio_device.mId;
        // audio_device.mState;
        //audio_device.SampleRate;
        
        audio_device.Clear();
        audio_device.Pause();
        audio_device.Play();
        audio_device.GetInfo();
        audio_device.Get();

        //TODO: Constrain these to their return types.
        /*{ audio_device.m_id } -> std::int32;*/
        /*{ audio_device.PrintInfo() } -> FAudioDeviceInfo;*/
        /*{ audio_device.Get() } -> std::span<std::byte>;*/
    };

}
