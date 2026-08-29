#include <SDL2/SDL.h>

#include <cstddef>
#include <concepts>

namespace Ark 
{
  template<typename U, std::integral T>
  inline constexpr std::size_t SizeofAudioBuffer(U bitDepth, T sampleRate)
  {
    return {};
  }

};
