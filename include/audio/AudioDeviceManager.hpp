#pragma once

#include "AudioDevice.hpp"

#include <memory>


namespace Ark 
{
  template<AudioDevice T>
  class UAudioDeviceManager
  {

    std::unique_ptr<T> mDevice;

  public: 

    UAudioDeviceManager();
    ~UAudioDeviceManager();

    UAudioDeviceManager(UAudioDeviceManager&) = delete;
    UAudioDeviceManager(UAudioDeviceManager&&) = delete;

    UAudioDeviceManager& operator=(UAudioDeviceManager&) = delete;
    UAudioDeviceManager& operator=(UAudioDeviceManager&&) = delete;
    

    [[nodiscard]] EAudioDeviceError EnumerateDevices() const;
    [[nodiscard]] EAudioDeviceError PauseDevice() const noexcept;
    [[nodiscard]] EAudioDeviceError ResetDevice() noexcept;
    [[nodiscard]] EAudioDeviceError InitializeDevice();

  };
}
