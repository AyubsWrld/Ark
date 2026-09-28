#include "WavWriter.hpp"

/**
 * @brief Writes the WAV file header.
 *
 * @param sample_rate Audio sample rate in Hz.
 * @param bits_per_sample Number of bits used per audio sample.
 * @param channels Number of audio channels.
 *
 * @return true if the header was written successfully, otherwise false.
 */
bool WavWriter::WriteHeader(
	std::uint32_t SampleRate,
	std::uint16_t BitsPerSample,
	std::uint16_t Channels)
{
	mFile.write("RIFF", 4);
	mFile.write("\0\0\0\0", 4); // Placeholder for file size
	mFile.write("WAVE", 4);
	mFile.write("fmt ", 4);

	const std::uint32_t SubChunkSize = 16;
	const std::uint16_t AudioFormat = 1; // PCM format
	const std::uint32_t ByteRate = SampleRate * Channels * BitsPerSample / 8;
	const std::uint16_t BlockAlign = Channels * BitsPerSample / 8;

	mFile.write(reinterpret_cast<const char*>(&SubChunkSize), 4);
	mFile.write(reinterpret_cast<const char*>(&AudioFormat), 2);
	mFile.write(reinterpret_cast<const char*>(&Channels), 2);
	mFile.write(reinterpret_cast<const char*>(&SampleRate), 4);
	mFile.write(reinterpret_cast<const char*>(&ByteRate), 4);
	mFile.write(reinterpret_cast<const char*>(&BlockAlign), 2);
	mFile.write(reinterpret_cast<const char*>(&BitsPerSample), 2);

	mFile.write("data", 4);
	mFile.write("\0\0\0\0", 4); // Placeholder for data size
	return true;
}

/**
 * @brief Writes floating-point audio samples to the WAV file.
 *
 * @param Data Pointer to the audio sample buffer.
 * @param Length Number of samples in the buffer.
 *
 * @return true if the audio data was written successfully, otherwise false.
 */
bool WavWriter::WriteAudio(
	const float* Data,
	std::size_t Length)
{
	/*
	for (std::size_t i = 0; i < Length; i++)
	{
		const std::int16_t Sample = static_cast<std::int16_t>(Data[i] * 32767);
		mFile.write(reinterpret_cast<const char*>(&Sample), sizeof(std::uint16_t));
		mDataSize += sizeof(std::uint16_t);
	}
	if (mFile.is_open())
	{
		mFile.seekp(4, std::ios::beg);
		std::uint32_t FileSize = 36 + mDataSize;
		mFile.write(reinterpret_cast<const char*>(&FileSize), 4);
		mFile.seekp(40, std::ios::beg);
		mFile.write(reinterpret_cast<const char*>(&FileSize), 4);
		mFile.seekp(0, std::ios::beg);
	}
	*/
	return true;
}

/**
 * @brief Opens an existing WAV file.
 *
 * @param Filename Path to the WAV file.
 *
 * @return true if the file was opened successfully, otherwise false.
 */
bool WavWriter::OpenWav(
	const std::string& Filename)
{
	if (mFilename != Filename) {
		if (mFile.is_open()) {
			mFile.close();
		}
	}
	if (!mFile.is_open()) {
		mFile.open(Filename, std::ios::binary);
		mFilename = Filename;
		mDataSize = 0;
	}
	return mFile.is_open();
}

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
bool WavWriter::Open(
	const std::string& Filename,
	std::uint32_t SampleRate,
	std::uint16_t BitsPerSample,
	std::uint16_t Channels)
{
	if (OpenWav(Filename))
	{
		WriteHeader(SampleRate, BitsPerSample, Channels);
	} else
	{
		return false;
	}
	return true;
}

/**
 * @brief Finalizes and closes the WAV file.
 *
 * @return true if the file was closed successfully, otherwise false.
 */
bool WavWriter::Close()
{
	mFile.close();
	return true;
}

/**
 * @brief Writes audio samples to the currently opened WAV file.
 *
 * @param Data Pointer to the audio sample buffer.
 * @param Length Number of samples in the buffer.
 *
 * @return true if the samples were written successfully, otherwise false.
 */
bool WavWriter::Write(
	const float* Data,
	std::size_t Length)
{
	return WriteAudio(Data, Length);
}

/**
 * @brief Finalizes and releases resources owned by the WAV writer.
 */
WavWriter::~WavWriter()
{
	if (mFile.is_open())
	{
		mFile.close();
	}
}
