#include "idt.h"
#include "common.h"

extern "C" void kbd_isr(void);
extern "C" void timer_isr(void);
extern "C" void pg_isr(void);
extern "C" void load_idt(u32i* idt_ptr);

struct IDT_descriptor IDT[IDT_SIZE];

extern "C" void set_gate_descriptor(u16i n, u32i address, u16i selector, u16i attr)
{
    IDT[n].offset_lowerbits = (address & 0xffff);
    IDT[n].selector = selector;
    IDT[n].zero = 0;
    IDT[n].type_attributes = attr;
    IDT[n].offset_higherbits = (address & 0xffff0000) >> 16;
}

extern "C" void IDT_init(void)
{
    u32i idt_ptr[2];
    u32i idt_address = (u32i)IDT;
    idt_ptr[0] = (sizeof(struct IDT_descriptor) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    set_gate_descriptor(0x21, (u32i)kbd_isr, 0x08, 0x8e);
    set_gate_descriptor(0x20, (u32i)timer_isr, 0x08, 0x8e);
    set_gate_descriptor(0x0e, (u32i)pg_isr, 0x08, 0x8e);

    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xff);
    outb(0xA1, 0xff);

    load_idt(idt_ptr);
}
