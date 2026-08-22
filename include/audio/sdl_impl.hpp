#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_log.h>

#include <atomic>
#include <cstdint>
#include <vector>
#include <mutex>

//
// SDL Audio capture
//

namespace Ark 
{
    class FAudioAsyncDevice 
    {
    public:
        FAudioAsyncDevice(int len_ms);

        ~FAudioAsyncDevice();

        /**
        * @brief Initializes the audio device backend
        *
        * @param Index of logical device used for capturing audio. 
        *
        * @return Whether initialization succeeded or not.
        *
        */
        
        [[nodiscard]] bool Init(int capture_id, int sample_rate);

        /**
        * @brief Initializes the audio device backend using the "best-fit" logical device for capturing.
        *
        * @param Index of logical device used for capturing audio. 
        *
        * @return Whether initialization succeeded or not.
        *
        */
        [[nodiscard]] bool Init(int sample_rate);

        [[nodiscard]] bool Resume();
        [[nodiscard]] bool Clear();
        [[nodiscard]] bool Pause();

        void Callback(uint8_t * stream, int len);

        void Get(int ms, std::vector<float> & audio);
        
        friend std::ostream& operator<<(std::ostream& o, const FAudioAsyncDevice& device);

    private:
        SDL_AudioDeviceID m_dev_id_in = 0;

        int m_len_ms = 0;
        int m_sample_rate = 0;

        std::atomic_bool m_running;
        std::mutex       m_mutex;

        std::vector<float> m_audio;
        size_t             m_audio_pos = 0;
        size_t             m_audio_len = 0;
    };
    // Return false if need to quit
    bool sdl_poll_events();
}
