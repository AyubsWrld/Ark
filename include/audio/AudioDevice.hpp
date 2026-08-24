#pragma once

#include <concepts>
#include <span>

namespace Ark 
{
  enum class EAudioDeviceError 
  {
    _max_
  }; 

  struct FAudioDeviceInfo {};

  class FAudioDevice 
  {
  public:

    virtual void              Resume()  = 0;
    virtual EAudioDeviceError Init()    = 0;
    virtual void              Clear()   = 0;
    virtual void              Pause()   = 0;

    virtual ~FAudioDevice();
  };


  // Does this fail to compile w/ private variables? 

  template<typename T>
  concept AudioDevice = requires (T audio_device)
  {
    audio_device.m_id;
    audio_device.Clear();
    audio_device.Pause();
    audio_device.PrintInfo();
    audio_device.Get();

    //TODO: Constrain these to their return types.
    /*{ audio_device.m_id } -> std::int32;*/
    /*{ audio_device.PrintInfo() } -> FAudioDeviceInfo;*/
    /*{ audio_device.Get() } -> std::span<std::byte>;*/
  };
}
