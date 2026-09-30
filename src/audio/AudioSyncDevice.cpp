#include "AudioSyncDevice.hpp"

#include "AudioAsyncDevice.hpp"
#include "WavWriter.hpp"

#include <algorithm>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include <iostream>

namespace Ark
{
	namespace
	{
		// TODO: Bounds checking for Fixed Size otherwise reallocations occur and we can write to the buffer indefinitely


		template <typename T, std::size_t Size>
		FixedBuffer<T, Size>::SizeType FixedBuffer<T, Size>::Write(
			T* Buffer,
			std::size_t Count)
		{
			for (std::size_t i = 0; i < Count; ++i)
			{
				DataBuffer[Elements + i] = Buffer[i];
			}
			Elements += Count;
			return Count;
		}

		template <typename T, std::size_t Size>
		FixedBuffer<T, Size>::SizeType FixedBuffer<T, Size>::Read(
			T* Buffer,
			std::size_t Count) const noexcept
		{
			for (std::size_t i = 0; i < Count; ++i)
			{
				Buffer[i] = DataBuffer[i];
			}
			return Count;
		}
	}

	EAudioDeviceError UAudioSyncDevice::Clear() noexcept
	{
		return {};
	}

	EAudioDeviceError UAudioSyncDevice::Init()
	{ // an opened audio device starts out paused
		// callback runs in a different thread from the main thread.

		if (SDL_Init(SDL_INIT_AUDIO) < 0)
		{
			spdlog::error("[{:s}]: Failed to Initialize Audio Subsytem: {}", __PRETTY_FUNCTION__, SDL_GetError());
			return EAudioDeviceError::SubsystemInitializationFailure;
		}

		SDL_AudioSpec capture_spec_desired, capture_spec_obtained;

		SDL_zero(capture_spec_desired);
		SDL_zero(capture_spec_obtained);

		capture_spec_desired.freq = 48000;
		capture_spec_desired.format = AUDIO_F32SYS;
		capture_spec_desired.samples = 1024;
		capture_spec_desired.channels = 1;
		capture_spec_desired.callback = [](void* userdata, uint8_t* stream, int len)
		{
			UAudioSyncDevice* audio = (UAudioSyncDevice*)userdata;
			audio->Callback(stream, len);
		};
		capture_spec_desired.userdata = this;

		mDeviceInfo.ID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, SDL_TRUE),
		                                     SDL_TRUE,
		                                     &capture_spec_desired,
		                                     &capture_spec_obtained,
		                                     0);

		mDeviceInfo.Name = SDL_GetAudioDeviceName(0, SDL_TRUE);
		mDeviceInfo.Format = SDL_GetAudioDeviceName(0, SDL_TRUE); // Swap see SDL_audio.h:123 for names...
		mDeviceInfo.Frequency = capture_spec_obtained.freq;
		mDeviceInfo.Size = capture_spec_obtained.size;
		mDeviceInfo.Samples = capture_spec_obtained.samples;
		mDeviceInfo.Channels = capture_spec_obtained.channels;

		if (!mDeviceInfo.ID) // TODO : this is all quite backend specific.
		{
			spdlog::error("[{:s}]: Failed to open audio device: {}", __PRETTY_FUNCTION__, SDL_GetError());
			return EAudioDeviceError::OpenFailure;
		}
		return EAudioDeviceError::Success;
	}

	FAudioDeviceInfo UAudioSyncDevice::GetInfo() const noexcept
	{
		return mDeviceInfo;
	}

	const float* UAudioSyncDevice::Get() const noexcept
	{
		return mBuffer.DataBuffer.data();
	}

	std::size_t UAudioSyncDevice::Size() const noexcept
	{
		return mBuffer.DataBuffer.size();
	}

	void UAudioSyncDevice::Pause() noexcept
	{
		spdlog::info("Attempting to play deviceID: {}\n", mDeviceInfo.ID);
		if (SDL_GetAudioDeviceStatus(mDeviceInfo.ID) == SDL_AUDIO_PAUSED)
		{
			return;
		}
		SDL_PauseAudioDevice(mDeviceInfo.ID, SDL_TRUE);
		mDeviceInfo.State = EAudioDeviceState::Paused;
	}

	void UAudioSyncDevice::Play() noexcept
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

	void UAudioSyncDevice::Callback(
		std::uint8_t* stream,
		int len) noexcept
	{
		std::size_t SampleSize = len / sizeof(float);
		if (mBuffer.WriteAvailable() >= SampleSize)
		{
			auto ToWrite = mBuffer.Write(reinterpret_cast<float*>(stream), SampleSize);
		}
		else
		{
			WavWriter Writer;
			Writer.Open("test_audio.wav", 48'000, 16, 1);
			Writer.WriteAudio(mBuffer.DataBuffer.data(), mBuffer.DataBuffer.size());
			Writer.Close();
			Pause();
		}
	}
}
