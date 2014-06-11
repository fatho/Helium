#ifndef _IDRISGC_H
#define _IDRISGC_H

#include "kernel/idris_rts/idris_rts.h"

void idris_gc(VM* vm);
void idris_gcInfo(VM* vm, int doGC);

#endif
