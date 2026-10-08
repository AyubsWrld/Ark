#pragma once

#include <piper.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

class PiperTTS
{
public:
    struct Audio
    {
        std::vector<float> samples;   // mono, float [-1, 1]
        int sampleRate = 0;
    };

    PiperTTS() = default;
    ~PiperTTS() { Reset(); }

    PiperTTS(const PiperTTS &) = delete;
    PiperTTS & operator=(const PiperTTS &) = delete;

    PiperTTS(PiperTTS && o) noexcept : synth_(std::exchange(o.synth_, nullptr)) {}
    PiperTTS & operator=(PiperTTS && o) noexcept
    {
        if (this != &o) { Reset(); synth_ = std::exchange(o.synth_, nullptr); }
        return *this;
    }

    // configPath may be empty -> defaults to modelPath + ".json"
    bool Load(const std::string & modelPath,
              const std::string & espeakDataPath,
              const std::string & configPath = "")
    {
        Reset();
        synth_ = piper_create(modelPath.c_str(),
                              configPath.empty() ? nullptr : configPath.c_str(),
                              espeakDataPath.c_str());
        return synth_ != nullptr;
    }

    bool IsLoaded() const { return synth_ != nullptr; }

    // lengthScale: 1.0 = normal, <1 faster, >1 slower
    // speakerId:  -1 = voice default (multi-speaker voices only)
	bool Synthesize(const std::string & text, Audio & out,
					float lengthScale = 1.0f, int speakerId = -1)
    {
    	if (!synth_) return false;

    	out.samples.clear();
    	out.sampleRate = 0;

    	piper_synthesize_options opts = piper_default_synthesize_options(synth_);
    	opts.length_scale = lengthScale;
    	if (speakerId >= 0) opts.speaker_id = speakerId;

    	const int startRc = piper_synthesize_start(synth_, text.c_str(), &opts);
    	if (startRc != PIPER_OK) {
    		std::fprintf(stderr, "PiperTTS: synthesize_start failed (%d)\n", startRc);
    		return false;
    	}

    	int chunks = 0;
    	for (;;)
    	{
    		piper_audio_chunk chunk{};   // zeroed each time, so stale pointers can't be reused
    		const int rc = piper_synthesize_next(synth_, &chunk);

    		if (rc != PIPER_OK && rc != PIPER_DONE) {
    			std::fprintf(stderr, "PiperTTS: synthesize_next failed (%d)\n", rc);
    			return false;
    		}

    		// Take samples whenever they're present, even on the DONE call
    		if (chunk.samples && chunk.num_samples > 0) {
    			++chunks;
    			if (chunk.sample_rate > 0) out.sampleRate = chunk.sample_rate;
    			out.samples.insert(out.samples.end(),
								   chunk.samples,
								   chunk.samples + chunk.num_samples);
    		}

    		if (rc == PIPER_DONE || chunk.is_last) break;
    	}

    	std::fprintf(stderr, "PiperTTS: %d chunk(s), %zu samples, %d Hz\n",
					 chunks, out.samples.size(), out.sampleRate);
    	return !out.samples.empty();
    }
    // 16-bit PCM mono WAV
    static bool SaveWav(const std::string & path, const Audio & audio)
    {
        std::ofstream f(path, std::ios::binary);
        if (!f) return false;

        const uint32_t dataBytes = (uint32_t)(audio.samples.size() * sizeof(int16_t));
        const uint32_t rate      = (uint32_t)audio.sampleRate;
        const uint16_t channels  = 1, bits = 16;
        const uint32_t byteRate  = rate * channels * bits / 8;
        const uint16_t blockAlign = channels * bits / 8;
        const uint32_t riffSize  = 36 + dataBytes;
        const uint32_t fmtSize   = 16;
        const uint16_t pcm       = 1;

        f.write("RIFF", 4); f.write((const char *)&riffSize, 4);
        f.write("WAVE", 4); f.write("fmt ", 4);
        f.write((const char *)&fmtSize, 4);
        f.write((const char *)&pcm, 2);
        f.write((const char *)&channels, 2);
        f.write((const char *)&rate, 4);
        f.write((const char *)&byteRate, 4);
        f.write((const char *)&blockAlign, 2);
        f.write((const char *)&bits, 2);
        f.write("data", 4); f.write((const char *)&dataBytes, 4);

        for (float s : audio.samples)
        {
            if (s > 1.f) s = 1.f; else if (s < -1.f) s = -1.f;
            const int16_t v = (int16_t)(s * 32767.f);
            f.write((const char *)&v, 2);
        }
        return (bool)f;
    }

private:
    void Reset()
    {
        if (synth_) { piper_free(synth_); synth_ = nullptr; }
    }

    piper_synthesizer * synth_ = nullptr;
};