#ifndef IDT_H
#define IDT_H

#define IDT_SIZE 256

struct IDT_descriptor
{
    unsigned short offset_lowerbits;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attributes;
    unsigned short offset_higherbits;
};

extern "C" void set_gate_descriptor(unsigned short n, unsigned int address, unsigned short selector, unsigned short attr);
extern "C" void IDT_init(void);

#endif
