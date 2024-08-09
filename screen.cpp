#include "screen.h"
#include "common.h"

#define DEC_DIGITS 10

static short* vga = (short*)0xb8000;
static short screen_x = 0;
static short screen_y = 0;

extern "C" void scroll_screen(void)
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

extern "C" void end_of_vga(void) //end of vga memory
{
    if (screen_y >= 25)
    {
        scroll_screen();
        screen_x = 0;
        screen_y = 24;
    }
}

extern "C" void move_cursor(void)
{
    unsigned short cursorLocation = screen_y * 80 + screen_x;
    outb(0x3D4, 14);
    outb(0x3D5, cursorLocation >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursorLocation);
}

extern "C" void put_char(char ch)
{
    if (ch == '\n')
    {
        screen_y++;
        screen_x = 0;
    }
    else if (ch == '\b')
    {
        if (screen_x == 0 && screen_y == 0) return;
        screen_x--;
        vga[screen_y * 80 + screen_x] = ' ';
    }
    else if (ch == '\t')
    {
        screen_x += 4;
    }
    else {
        vga[screen_y * 80 + screen_x] = 0x0700 | ch;
        screen_x++;
    }

    if (screen_x > 80)
    {
        screen_y++;
        screen_x = 0;
    }
    move_cursor();
    end_of_vga();
}

extern "C" void print_dec(int n)
{
    if (n == 0)
    {
        put_char('0');
        return;
    }
    char buf[DEC_DIGITS] = { 0 };
    int cnt = 0;
    while (n != 0)
    {
        buf[cnt] = char(n % 10 + 48);
        n /= 10;
        ++cnt;
    }
    int i = cnt - 1;
    int k = cnt / 2;
    for (int j = 0; j < k; ++j, --i)
    {
        buf[j] ^= buf[i];
        buf[i] ^= buf[j];
        buf[j] ^= buf[i];
    }

    for (int j = 0; j < cnt; ++j)
    {
        put_char(buf[j]);
    }
}

extern "C" void print_hex(unsigned int dec_n)
{
    print_str("0x");
    if (dec_n == 0)
    {
        put_char('0');
        return;
    }
    u32i hex_GCD = 268435456;//4294967295;//16777216; //k = 16776960 268435456;
    const char* hex_nums = "0123456789ABCDEF";
    while (dec_n != 0)
    {
        put_char(*(hex_nums + dec_n / hex_GCD));
        dec_n %= hex_GCD;
        hex_GCD /= 16;
    }
}

extern "C" void print_str(const char* s)
{
    while (*s != 0)
    {
        if (*s != '\n')
        {
            put_char(*s);
        }
        else
        {
            screen_y++;
            screen_x = 0;
            move_cursor();
        }
        ++s;
    }
}

extern "C" void clear_screen(void)
{
    unsigned int i = 0;
    while (i < 80 * 25 * 2)
    {
        vga[i] = ' ';
        vga[i + 1] = 0x0700;
        i += 2;
    }
    screen_x = 0;
    screen_y = 0;
    move_cursor();
}
