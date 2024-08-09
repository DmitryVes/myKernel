#include "time.h"

static unsigned int time = 0;

extern "C" void timer_init(void)
{
    outb(0x20, 0xb0);
}

extern "C" void timer_handler(void)
{
    ++time;
    outb(0x20, 0x20);
}

extern "C" void ksleep(u32i ticks)
{
    u32i cur_ticks = time;
    while ((time - cur_ticks) != ticks);
}
