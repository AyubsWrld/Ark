/**
* @file WavWriter.hpp
 * @brief Declaration of the WavWriter class for writing PCM audio data to WAV files.
 *
 * @author 0xtrensetta
 * @date 2026-09-27
 * @version 0.1.0
 *
 * @copyright
 * Copyright (c) 2026 0xtrensetta.
 *
 * @details
 * Provides functionality for opening WAV files, writing WAV headers,
 * writing floating-point audio samples, and finalizing the output file.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <fstream>

/**
 * @brief Writes PCM audio data to a WAV file.
 */
class WavWriter
{
	std::ofstream mFile;
	std::uint32_t mDataSize = 0;
	std::string mFilename;

public:
	/**
     * @brief Constructs an empty WAV writer.
     */
	WavWriter() = default;

	/**
     * @brief Writes the WAV file header.
     *
     * @param SampleRate Audio sample rate in Hz.
     * @param BitsPerSample Number of bits used per audio sample.
     * @param Channels Number of audio channels.
     *
     * @return true if the header was written successfully, otherwise false.
     */
	bool WriteHeader(std::uint32_t SampleRate, std::uint16_t BitsPerSample, std::uint16_t Channels);

	/**
     * @brief Writes floating-point audio samples to the WAV file.
     *
     * @param Data Pointer to the audio sample buffer.
     * @param Length Number of samples in the buffer.
     *
     * @return true if the audio data was written successfully, otherwise false.
     */
	bool WriteAudio(const float* Data, std::size_t Length);

	/**
     * @brief Opens an existing WAV file.
     *
     * @param Filename Path to the WAV file.
     *
     * @return true if the file was opened successfully, otherwise false.
     */
	bool OpenWav(const std::string& Filename);

	/**
     * @brief Opens a WAV file and initializes its audio format.
     *
     * @param Filename Path to the output WAV file.
     * @param SampleRate Audio sample rate in Hz.
     * @param BitsPerSample Number of bits used per audio sample.
     * @param Channels Number of audio channels.
     *
     * @return true if the file was opened successfully, otherwise false.
     */
	bool
	Open(const std::string& Filename, std::uint32_t SampleRate, std::uint16_t BitsPerSample, std::uint16_t Channels);

	/**
     * @brief Finalizes and closes the WAV file.
     *
     * @return true if the file was closed successfully, otherwise false.
     */
	bool Close();

	/**
     * @brief Writes audio samples to the currently opened WAV file.
     *
     * @param Data Pointer to the audio sample buffer.
     * @param Length Number of samples in the buffer.
     *
     * @return true if the samples were written successfully, otherwise false.
     */
	bool Write(const float* Data, std::size_t Length);

	/**
     * @brief Finalizes and releases resources owned by the WAV writer.
     */
	~WavWriter();
};
