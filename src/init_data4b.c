#if defined THINK_C
#include "gnushogi.h"
#endif


next_array prook_nextpos, pbishop_nextpos, wgold_nextpos;

#ifndef SAVE_DISTDATA
small_short distdata[NO_SQUARES][NO_SQUARES];
#endif

