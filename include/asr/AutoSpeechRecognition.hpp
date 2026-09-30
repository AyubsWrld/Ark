#pragma once

#include <whisper.h>
#include <spdlog/spdlog.h>
#include <string>

enum class EAutoSpeechRecognitionError
{
	Success,
	UnspecifiedError,
	InitializationError,
	_max
};

class AutoSpeechRecognition
{
	const char* ModelPath;
	const char* WavPath;
	whisper_context_params ContextParams;
	whisper_full_params Params;
	whisper_context* Context;

public:
	AutoSpeechRecognition() = default;
	~AutoSpeechRecognition() = default;

	AutoSpeechRecognition(AutoSpeechRecognition&) = delete;
	AutoSpeechRecognition(AutoSpeechRecognition&&) = delete;

	AutoSpeechRecognition& operator=(AutoSpeechRecognition&) = delete;
	AutoSpeechRecognition& operator=(AutoSpeechRecognition&&) = delete;

	[[nodiscard]] EAutoSpeechRecognitionError Init()
	{
		using enum EAutoSpeechRecognitionError;
		ContextParams = whisper_context_default_params();
		Context = whisper_init_from_file_with_params(ModelPath, ContextParams);

		if (!Context)
		{
			spdlog::error("{:s}: Failed to initialize ASR pipeline");
			Cleanup();
			return InitializationError;
		}
		return Success;
	};

	[[nodiscard]] std::string Execute() noexcept;

	[[nodiscard]] void Cleanup() noexcept
	{
		whisper_free(Context);
	};
};
