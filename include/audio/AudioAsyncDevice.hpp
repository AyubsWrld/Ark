#pragma once
#include <concepts>

template<typename T> 
concept IAudioDevice = requires(T audio_device)
{
  audio_device.Init();      // Initializes device. 
  audio_device.Pause();     // Pauses capturing. 
  audio_device.Resume();    // Resume capturing. 
  audio_device.Clear();     // Clears underlying buffer.
  audio_device.Get();       // Retrieves an underlying buffer.
  audio_device.Callback();  // Callback to run everytime audio is sampled.
  audio_device.m_dev_id_in; // DeviceID
};

