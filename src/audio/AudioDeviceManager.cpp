#include "AudioDeviceManager.hpp"

 
// FAudioDeviceManager

namespace Ark 
{
  
  template<AudioDevice T>
  UAudioDeviceManager<T>::UAudioDeviceManager() {}

  template<AudioDevice T>
  UAudioDeviceManager<T>::~UAudioDeviceManager() {}


  [[nodiscard]] EAudioDeviceError UAudioDeviceManager<T>::EnumerateDevices() const
  {
  }
};
