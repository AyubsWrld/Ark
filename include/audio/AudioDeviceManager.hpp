#pragma once

#include "AudioDevice.hpp"

#include <SDL.h>
#include <SDL2/SDL_audio.h>
#include <cstdint>
#include <memory>
#include <spdlog/spdlog.h>
//#include <tracy/Tracy.hpp>

// TODO: remove just for protyping
struct FAudioDevice
{
    void cb (uint8_t *stream, int len)
    {
        // std::cout << "Captured " << len << " samples\n";
    }
    std::size_t m_id;

    std::vector<uint8_t> mBuffer;
    std::size_t mId;
    int mState;
    int SampleRate;

    void Clear () { return; }
    void Pause () { return; }
    void Resume () { return; }
    void GetInfo () { return; }
    void Get () { return; }
    void PrintInfo () { return; }
};

namespace Ark
{
    enum
    {
        OUTPUT = 0,
        CAPTURE = 1
    };

    enum
    {
        SDL_ERROR = 98 // TODO: remove
    };

    template <AudioDevice T> class UAudioDeviceManager
    {
        std::unique_ptr<T> mDevice;

      public:
        UAudioDeviceManager ();
        ~UAudioDeviceManager ();

        UAudioDeviceManager (UAudioDeviceManager &) = delete;
        UAudioDeviceManager (UAudioDeviceManager &&) = delete;

        UAudioDeviceManager &operator= (UAudioDeviceManager &) = delete;
        UAudioDeviceManager &operator= (UAudioDeviceManager &&) = delete;

        [[nodiscard]] EAudioDeviceError PauseDevice () const noexcept;
        [[nodiscard]] EAudioDeviceError ResetDevice () noexcept;
        [[nodiscard]] EAudioDeviceError InitializeDevice ();
        void EnumerateDevices (bool capture) const noexcept;
        void EnumerateCaptureDevices () const noexcept;

        void EnumerateOutputDevices () const noexcept;
    };

    template <AudioDevice T> UAudioDeviceManager<T>::UAudioDeviceManager ()
    {
        spdlog::info ("{}\n", __PRETTY_FUNCTION__);
        if (SDL_Init (SDL_INIT_AUDIO) < 0)
        {
            SDL_LogError (SDL_LOG_CATEGORY_APPLICATION,
                          "Couldn't initialize SDL: %s\n", SDL_GetError ());
        }
    }

    template <AudioDevice T> UAudioDeviceManager<T>::~UAudioDeviceManager ()
    {
        SDL_Quit ();
    }

    template <AudioDevice T>
    void UAudioDeviceManager<T>::EnumerateDevices (bool capture) const noexcept
    {
        // could this be elided?
        return capture ? EnumerateCaptureDevices() : EnumerateOutputDevices();
    }

    template <AudioDevice T>
    void UAudioDeviceManager<T>::EnumerateCaptureDevices () const noexcept
    {
        if (const std::int32_t nDevices = SDL_GetNumAudioDevices (CAPTURE);
            nDevices != SDL_ERROR)
        {
            for (std::size_t i{}; i < nDevices; i++)
                spdlog::info ("Found {}",
                              SDL_GetAudioDeviceName (0, SDL_TRUE));
        }
    }

    template <AudioDevice T>
    void UAudioDeviceManager<T>::EnumerateOutputDevices () const noexcept
    {
        if (const std::int32_t nDevices = SDL_GetNumAudioDevices (OUTPUT);
            nDevices != SDL_ERROR)
        {
            for (std::size_t i{}; i < nDevices; i++)
                spdlog::info ("Found {}",
                              SDL_GetAudioDeviceName (0, SDL_TRUE));
        }
        return;
    }
}
