#pragma once
#include <llama.h>
#include <string>
#include <format>
#include <expected>

int Chat(std::string model_path, std::string prompt);

using FLlamaModelParams = llama_model_params;
using FLlamaModel		= llama_model* ;

namespace Ark
{

    struct [[nodiscard]] FContextParameters 
    {
        std::size_t BatchSize;
        std::size_t ContextSize;

    };

	enum class ELanguageSynthesizerError
	{
		Success,
		InitializationFailure,
		LoadModelFailure,
		CreateContextFailure,
        CreateSamplerFailure,
        InvalidState,
        DecodeFailure,
        TokenizationFailure,
		_max
	};

	class ULanguageSynthesizer
	{

		std::string			ModelPath = "/";
		llama_model*		Model = nullptr;
		llama_context*	ModelContext = {};
		llama_model_params	ModelParams = {};
		llama_vocab*		ModelVocab	= {}; // Useful? 

	public:

		ULanguageSynthesizer(std::string_view ModelPath);
		~ULanguageSynthesizer() noexcept;

		ULanguageSynthesizer(const ULanguageSynthesizer&) = delete;
		ULanguageSynthesizer(ULanguageSynthesizer&&) = delete;

		ULanguageSynthesizer& operator=(const ULanguageSynthesizer&) = delete;
		ULanguageSynthesizer& operator=(ULanguageSynthesizer&&) = delete;


    private:

		[[nodiscard]] ELanguageSynthesizerError LoadModel(std::string_view ModelPath);
		[[nodiscard]] ELanguageSynthesizerError CreateContext(FContextParameters ContextParams = FContextParameters{0,0}) noexcept;
		[[nodiscard]] ELanguageSynthesizerError CreateContext(std::size_t PromptTokenCount, std::size_t MaxGeneratedTokens) noexcept;
		[[nodiscard]] std::vector<llama_token> Tokenize(std::string_view Prompt) const noexcept;


		void SetVocab();

	};

};

template <>
struct std::formatter<llama_model_params>
{
	constexpr auto  parse(
		std::format_parse_context& context)
	{
		auto it = context.begin();
		return it;
	}

	auto format(
		const llama_model_params& params,
		std::format_context& context) const
	{
		return std::format_to(context.out(), "{}", params.main_gpu);
	}
};
