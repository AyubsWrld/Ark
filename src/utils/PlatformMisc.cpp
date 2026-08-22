#include "PlatformMisc.hpp"

void FPlatformMisc::DebugBreak() 
{
#ifdef DEBUG
  __builtin_debugtrap(); 
#endif
}

