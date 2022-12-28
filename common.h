#ifndef COMMON_H
#define COMMON_H

#define NULL 0
//#define CLOCK_FREQUENCY 1193180

typedef int s32int;
typedef unsigned int u32int;
typedef short s16int;
typedef unsigned short u16int;
typedef char s8int;
typedef unsigned char u8int;

extern "C" void outb(u16int port, u8int value);
extern "C" u8int inb(u16int port);
extern "C" u16int inw(u16int port);

#endif 
