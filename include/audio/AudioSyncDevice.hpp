/**
* @file AudioSyncDevice.hpp
 * @brief Declaration of the AudioSyncDevice class for Syncrhonous Audio Device Capture
 *
 * @author 0xtrensetta
 * @date 2026-09-27
 * @version 0.1.0
 *
 * @copyright
 * Copyright (c) 2026 0xtrensetta.
 *
 * @details
 * Provides functionality for opening an Audio Device for capturing
 * and writing the captured audio to a fixed PCM buffer which can be later dumped to a file.
 */

#pragma once

#include "AudioDevice.hpp"

#include <vector>

namespace Ark
{
	namespace
	{

		template <typename T, std::size_t Size = 1024>
		struct FixedBuffer
		{
			using SizeType = std::vector<T>::size_type;
			using IteratorType = std::vector<T>::iterator;

			std::vector<T> DataBuffer = std::vector<T>(Size);
			SizeType Capacity = Size;
			SizeType Elements = 0;

			FixedBuffer() { DataBuffer.resize(Size); }
			~FixedBuffer() = default;

			[[nodiscard]] SizeType Write(T* Buffer, std::size_t Count);
			[[nodiscard]] SizeType Read(T* Buffer, std::size_t Count) const noexcept;

			[[nodiscard]] bool bIsEmpty() const noexcept { return DataBuffer.empty(); }
			[[nodiscard]] SizeType WriteAvailable() const noexcept { return Capacity - Elements; }

		};
		constexpr std::size_t SAMPLE_RATE = 48'000;
		constexpr std::size_t DURATION_SECONDS = 2;

		constexpr std::size_t BUFFER_SIZE =
			SAMPLE_RATE * DURATION_SECONDS;

	}

	class UAudioSyncDevice
	{

        FAudioDeviceInfo					mDeviceInfo;
		FixedBuffer<float,BUFFER_SIZE>		mBuffer;

	public:

		UAudioSyncDevice() = default;
		~UAudioSyncDevice() = default;

		UAudioSyncDevice(UAudioSyncDevice&) = delete;
		UAudioSyncDevice(UAudioSyncDevice&&) = delete;

		UAudioSyncDevice& operator=(UAudioSyncDevice&) = delete;
		UAudioSyncDevice& operator=(UAudioSyncDevice&&) = delete;

		[[nodiscard]] EAudioDeviceError Clear()    noexcept;
		[[nodiscard]] EAudioDeviceError Init();
		[[nodiscard]] FAudioDeviceInfo  GetInfo() const noexcept;

        std::span<float> Get() const noexcept;
		void Pause()    noexcept;
		void Play()   noexcept;
		void Callback(uint8_t * stream, int len) noexcept;

	};
};
