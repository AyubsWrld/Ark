#pragma once

#include <whisper.h>
#include <spdlog/spdlog.h>
#include <expected>
#include <string>
#include <vector>

namespace Ark
{
	enum class ESpeechRecognizerError
	{
		Success,
		UnspecifiedError,
		PipelineError,
		InitializationError,
		_max
	};

	struct FSpeechRecognizerConfig
	{
		const char* ModelPath;
	};

	class USpeechRecognizer
	{
		const char* ModelPath;
		const char* WavPath = "test_audio.wav";
		whisper_context_params ContextParams;
		whisper_full_params TranscriptionParams;
		whisper_context* Context;

	public:
		USpeechRecognizer(const char* ModelPath);
		~USpeechRecognizer();

		USpeechRecognizer(USpeechRecognizer&) = delete;
		USpeechRecognizer(USpeechRecognizer&&) = delete;

		USpeechRecognizer& operator=(USpeechRecognizer&) = delete;
		USpeechRecognizer& operator=(USpeechRecognizer&&) = delete;

		[[nodiscard]] std::expected<std::string, ESpeechRecognizerError>
		Transcribe(std::vector<float>& Samples) noexcept;

		[[nodiscard]] ESpeechRecognizerError Init();
		[[nodiscard]] ESpeechRecognizerError SetContext() noexcept;
		[[nodiscard]] ESpeechRecognizerError LoadModel() noexcept;
		[[nodiscard]] std::string Execute() noexcept;
		void Cleanup() noexcept;
	};
}
