
#include "LanguageSynthesizer.hpp"
#include <llama.h>
#include <spdlog/spdlog.h>

static std::string BuildPrompt(llama_model * model, const std::string & user)
{
	const char * tmpl = llama_model_chat_template(model, nullptr);

	llama_chat_message msgs[] = { { "user", user.c_str() } };

	std::vector<char> buf(user.size() * 2 + 1024);
	int n = llama_chat_apply_template(tmpl, msgs, 1, /*add_ass=*/true,
									  buf.data(), (int)buf.size());
	if (n > (int)buf.size()) {
		buf.resize(n);
		n = llama_chat_apply_template(tmpl, msgs, 1, true,
									  buf.data(), (int)buf.size());
	}

	std::string out;
	if (n < 0) {
		// fallback: plain ChatML
		out = "<|im_start|>user\n" + user + "<|im_end|>\n<|im_start|>assistant\n";
	} else {
		out.assign(buf.data(), n);
	}

	out += "<think>\n\n</think>\n\n";   // empty think block = no reasoning
	return out;
}

static void QuietLog(enum ggml_log_level level, const char * text, void * /*user*/)
{
	if (level >= GGML_LOG_LEVEL_ERROR) {
		std::fputs(text, stderr);
	}

}

namespace Ark
{
	ULanguageSynthesizer::ULanguageSynthesizer(std::string_view ModelPath)
	{
		ModelParams = llama_model_default_params();
		if (LoadModel(ModelPath) != ELanguageSynthesizerError::Success)
		{
			spdlog::error("[{}]: Failed to load model: {}", __FUNCTION__, ModelPath);
			std::exit(EXIT_FAILURE);
		}

		if (CreateContext(FContextParameters{.BatchSize = 32, .ContextSize = 32 }) != ELanguageSynthesizerError::Success)
		{
			spdlog::error("[{}]: Failed to create model context", __FUNCTION__);
			llama_model_free(Model);
			std::exit(EXIT_FAILURE);
		}
	}

    ULanguageSynthesizer::~ULanguageSynthesizer() noexcept
    {
        /*
        if (Sampler)
        {
            llama_sampler_free(Sampler);
            Sampler = nullptr;
        }

        if (ModelContext)
        {
            llama_free(ModelContext);
            ModelContext = nullptr;
        }
        */

        if (Model)
        {
            llama_model_free(Model);
            Model = nullptr;
        }
    }

	[[nodiscard]] ELanguageSynthesizerError ULanguageSynthesizer::LoadModel(std::string_view Path)
	{
		Model = llama_model_load_from_file(Path.data(), ModelParams);
		return Model ? ELanguageSynthesizerError::Success : ELanguageSynthesizerError::LoadModelFailure;
	}

	void SetVocab()
	{
		return;
	}


    [[nodiscard]] std::vector<llama_token> ULanguageSynthesizer::Tokenize(std::string_view Prompt) const noexcept
    { 
        // Since the language synthesizer owns the model and therefore controls it's lifetime,
        // it should by extension own the opaque pointer to the models vocabulary. 
        const llama_vocab* Vocabulary = llama_model_get_vocab(Model);

        auto nTokens = -llama_tokenize(
                Vocabulary,
                Prompt.data(),
                Prompt.size(), 
                nullptr, 
                0,
                false,
                false
                );

        std::vector<llama_token> Tokens(nTokens);

        if (llama_tokenize(
                Vocabulary,
                Prompt.data(),
                Prompt.size(), 
                Tokens.data(), 
                Tokens.size(),
                true,
                true 
                ) < 0)
        {
            spdlog::error("Failed to tokenize string\n");
            return Tokens;
        }
        return Tokens;
    }

    // Todo: Handle potential overflow. use direct forms of initialization {} as opposed to indirect forms = ...
    [[nodiscard]] ELanguageSynthesizerError ULanguageSynthesizer::CreateContext(FContextParameters ContextParams) noexcept 
    {
        if (!Model) 
        {
            spdlog::error("Failed to create model context, model is not loaded");
            return ELanguageSynthesizerError::CreateContextFailure;
        }
        llama_context_params ContextParameters = llama_context_default_params();
        ContextParameters.n_ctx    = ContextParams.ContextSize;
        ContextParameters.n_batch  = ContextParams.BatchSize;
        ModelContext = llama_init_from_model(Model, ContextParameters);
        if (!ModelContext) 
        {
            spdlog::error("Failed to create model context");
            return ELanguageSynthesizerError::CreateContextFailure;
        }
        return ELanguageSynthesizerError::Success;
    }

    [[nodiscard]] ELanguageSynthesizerError ULanguageSynthesizer::CreateContext(std::size_t PromptTokenCount, std::size_t MaxGeneratedTokens) noexcept
    {
        if (!Model) 
        {
            spdlog::error("Failed to create model context, model is not loaded");
            return ELanguageSynthesizerError::CreateContextFailure;
        }

        llama_context_params ContextParameters = llama_context_default_params();
        ContextParameters.n_ctx    = PromptTokenCount + MaxGeneratedTokens - 1;
        ContextParameters.n_batch  = PromptTokenCount;
        ModelContext = llama_init_from_model(Model, ContextParameters);
        if (!ModelContext) 
        {
            spdlog::error("Failed to create model context");
            return ELanguageSynthesizerError::CreateContextFailure;
        }
        return ELanguageSynthesizerError::Success;
    }
}

