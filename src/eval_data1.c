#if defined THINK_C
#include "gnushogi.h"
#endif

#ifdef CACHE
#if defined THINK_C
etable_field *etab[2];
#else
etable_field etab0, etab1;
etable_field *etab[2] = 
  { &etab0, &etab1 };
#endif
#endif
