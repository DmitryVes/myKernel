#include "common.h"
#include "kbdmap.h"

#define IDT_SIZE 256

static short* vga = (short*)0xb8000;
static int screen_x = 0;
static int screen_y = 0;

extern "C" void res_stack(void);
extern "C" void kbd_handler(void);
extern "C" void load_idt(unsigned long* idt_ptr);

extern "C" void scroll_screen()
{
    for (int i = 0; i < 24 * 80; ++i)
    {
        vga[i] = vga[i + 80];
    }
    for (int i = 24 * 80; i < 25 * 80; ++i)
    {
        vga[i] = ' ';
    }
}

extern "C" void move_cursor()
{
    u16int cursorLocation = screen_y * 80 + screen_x;
    outb(0x3D4, 14);
    outb(0x3D5, cursorLocation >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursorLocation);
}

extern "C" void print_str(const char* s)
{
    while (*s != 0)
    {
        vga[screen_y * 80 + screen_x++] = 0x0700 | *s++;
        move_cursor();
    }
}

extern "C" void print_handler()
{
    outb(0x20, 0x20);

    if ((inb(0x64) & 0x01) != 0x01)
    {
        return;
    }

    short scan_code = inb(0x60);

    if (scan_code > 89)
    {
        return;
    }

    char kbd_char = kbd_US[scan_code];

    if (kbd_char == '\n')
    {
        screen_x = 0;
        screen_y++;
    }
    else if (kbd_char == '\b')
    {
        screen_x--;
        vga[screen_y * 80 + screen_x] = ' ';
    }
    else if (kbd_char == '\t')
    {
        screen_x <<= 1;
    }
    else if (kbd_char != 0)
    {
        vga[screen_y * 80 + screen_x] = 0x0700 | kbd_char;
        screen_x++;
        if (screen_y >= 25)
        {
            scroll_screen();
            screen_x = 0;
            screen_y = 24;
        }
    }
    if (screen_x >= 80)
    {
        screen_x = 0;
        screen_y++;
    }
    move_cursor();
}

extern "C" void memset(unsigned long* dest, int ch, unsigned long cnt)
{
    volatile unsigned long* start = dest;
    for (unsigned long i = 0; i < cnt; ++i)
    {
        start[i] = ch;
    }
}

struct IDT
{
    unsigned short offset_lowerbits;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attributes;
    unsigned short offset_higherbits;
};

struct IDT IDT_deskriptors[IDT_SIZE];

extern "C" void IDT_init()
{
    unsigned long idt_ptr[2];
    unsigned long idt_address = (unsigned long)IDT_deskriptors;
    idt_ptr[0] = (sizeof(struct IDT) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    unsigned long kbd_address = (unsigned long)kbd_handler;
    IDT_deskriptors[0x21].offset_lowerbits = (kbd_address & 0xffff);
    IDT_deskriptors[0x21].offset_higherbits = (kbd_address & 0xffff0000) >> 16;
    IDT_deskriptors[0x21].zero = 0;
    IDT_deskriptors[0x21].selector = 0x08;
    IDT_deskriptors[0x21].type_attributes = 0x8e;

    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04); 
    outb(0xA1, 0x02); 

    outb(0x21, 0x01);
    outb(0xA1, 0x01);


    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    load_idt(idt_ptr);
}

extern "C" void kbd_init()
{
    outb(0x21, 0xFD); //enable kbd on IRQ1
}

extern "C" void outb(u16int port, u8int value)
{
    asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

extern "C" u8int inb(u16int port)
{
    u8int ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

extern "C" u16int inw(u16int port)
{
    u16int ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

extern "C" int strlen(const char* str)
{
    int length = 0;
    while (str[length] != '\0')
    {
        ++length;
    }
    return length;
}

extern "C" void itos(int n)
{
    while (n != 0)
    {
        vga[screen_y * 80 + screen_x] = 0x0700 | char(n % 10 + 48);
        screen_x++;
        move_cursor();
        n /= 10;
    }
}

extern "C" void turn_to_hex(u32int dec_n)
{
    const char* hex_nums = "0123456789ABCDEF\0";
    int hex_GCD = 16777216; //k = 16776960;
        itos(*(hex_nums + (dec_n / hex_GCD)));
        dec_n %= hex_GCD;
        hex_GCD /= 16;
    }
}

extern "C" void clear_screen()
{
    unsigned int j = 0;
    while (j < 80 * 25 * 2)
    {
        vga[j] = ' ';
        vga[j + 1] = 0x07;
        j = j + 2;
    }
}

extern "C" void kmain(void)
{
    IDT_init();
    kbd_init();
    while (1);
}
