/*=============================================================================
	PlatformMisc.hpp: implementations of misc functions
=============================================================================*/

#define ARK_DEBUG_BREAK()           \
  do                                \
  {                                 \
    FPlatformMisc::DebugBreak();    \
  } while(false)

// ??
#ifndef ARK_DEBUG_BREAK
#error ARK_DEBUG_BREAK is not defined for this platform
#endif

struct FPlatformMisc
{
  static void DebugBreak() ;
};




