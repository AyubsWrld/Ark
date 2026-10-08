#include "AutoSpeechRecognition.hpp"
#include <sstream>

namespace Ark
{
	USpeechRecognizer::USpeechRecognizer(const char* ModelPath)
		: ModelPath(ModelPath)
	{
		if (Init() != ESpeechRecognizerError::Success)
		{

		}
	}

	USpeechRecognizer::~USpeechRecognizer()
	{
		Cleanup();
	}

	ESpeechRecognizerError USpeechRecognizer::Init()
	{
		using enum ESpeechRecognizerError;
		ContextParams = whisper_context_default_params();
		Context = whisper_init_from_file_with_params(ModelPath, ContextParams);

		if (!Context)
		{
			spdlog::error("{:s}: Failed to initialize ASR pipeline");
			Cleanup();
			return InitializationError;
		}
		TranscriptionParams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
		return Success;
	};

	[[nodiscard]] std::expected<std::string, ESpeechRecognizerError> USpeechRecognizer::Transcribe(std::vector<float>& Samples) noexcept
	{
		std::stringstream Text;
		if (whisper_full(
			Context,
			TranscriptionParams,
			Samples.data(),
			Samples.size()) != 0)
		{
			return std::unexpected(ESpeechRecognizerError::PipelineError);
		}
		int Segments = whisper_full_n_segments(Context);

		for (int SegmentIndex = 0; SegmentIndex < Segments; SegmentIndex++ )
		{
			Text << whisper_full_get_segment_text(Context, SegmentIndex) << " ";
		}
		return Text.str();
	}

	[[nodiscard]] void USpeechRecognizer::Cleanup() noexcept
	{
		whisper_free(Context);
	};
}
