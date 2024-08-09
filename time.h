#ifndef TIME_H
#define TIME_H

#include "common.h"

extern "C" void timer_init(void);
extern "C" void timer_handler(void);
extern "C" void ksleep(u32i ticks);

#endif
