#include "common.h"

extern "C" void outb(u16i port, u8i value)
{
    asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

extern "C" u8i inb(u16i port)
{
    u8i ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

extern "C" u16i inw(u16i port)
{
    u16i ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

extern "C" u8i lob(u32i n)
{
    n &= 0xF;
    return n;
}

extern "C" u8i hib(u32i n)
{
    n &= 0xF0;
    return n;
}
