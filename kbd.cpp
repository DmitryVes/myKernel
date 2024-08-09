#include "common.h"
#include "screen.h"
#include "kbd.h"
#include "kbdmap.h"

extern "C" void kbd_init(void)
{
    outb(0x21, 0xFD);
}

extern "C" void print_handler(void)
{
    outb(0x20, 0x20);

    if ((inb(0x64) & 0x01) != 0x01)
    {
        return;
    }

    short scan_code = inb(0x60);
    char kbd_char = kbd_US[scan_code];

    if (scan_code == 42) {
        do
        {
            scan_code = inb(0x60);
            kbd_char = shift_kbd_US[scan_code];
            if (kbd_char != 0 && scan_code < 89)
            {
                outb(0x60, 0xF3);
                outb(0x60, 0x00);
                if ((inb(0x64) & 0x01) == 0x01) put_char(kbd_char);
            }
        }
        while (scan_code != 42 + 0x80);
    }

    if (kbd_char != 0 && scan_code < 89)
    {
        put_char(kbd_char);
    }
}
