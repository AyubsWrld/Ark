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

	enum class EAudioDeviceBackend : std::uint8_t
	{
		SDL,
		_max
	};

    struct FAudioDeviceInfo
    {
        std::string     Name;           // Device Name
        std::string     Format;         // Audio data format
        std::size_t     Frequency;      // DSP frequency -- samples per second
        std::size_t     Size;           // Audio buffer size in bytes (calculated)
        std::uint16_t   Samples;        // Audio buffer size in sample FRAMES (total samples divided by channel count) */
        std::uint8_t    Channels;       // Number of channels: 1 mono, 2 stereo */
    	std::uint8_t	ID;				// Audio Device Identifier, holds different semantic meaning based on backend used.
    	EAudioDeviceBackend Backend;	// Audio Device Backend used.
    	EAudioDeviceState State;		// Audio Device State
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
