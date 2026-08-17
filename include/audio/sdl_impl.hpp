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
    class FAudioAsyncDevice {
    public:
        FAudioAsyncDevice(int len_ms);
        ~FAudioAsyncDevice();

        bool Init(int capture_id, int sample_rate);

        bool Resume();
        bool Pause();
        bool Clear();

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
