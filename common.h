#ifndef COMMON_H
#define COMMON_H

#define NULL 0

typedef int s32i;
typedef unsigned int u32i;
typedef short s16i;
typedef unsigned short u16i;
typedef char s8i;
typedef unsigned char u8i;

extern "C" void outb(u16i port, u8i value);
extern "C" u8i inb(u16i port);
extern "C" u16i inw(u16i port);
extern "C" u8i lob(u32i n);
extern "C" u8i hib(u32i n);

#endif 
