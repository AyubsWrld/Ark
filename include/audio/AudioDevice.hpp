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
        _max
    }; 

    struct FAudioDeviceInfo 
    {
        std::string Name;
    };

    template<typename T>
    concept AudioDevice = requires (T audio_device)
    {
        audio_device.mBuffer;
        audio_device.mId;
        audio_device.SampleRate;
        audio_device.Clear();
        audio_device.Pause();
        audio_device.Resume();
        audio_device.GetInfo();
        audio_device.Get();

        //TODO: Constrain these to their return types.
        /*{ audio_device.m_id } -> std::int32;*/
        /*{ audio_device.PrintInfo() } -> FAudioDeviceInfo;*/
        /*{ audio_device.Get() } -> std::span<std::byte>;*/
    };

}
