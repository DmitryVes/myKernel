#ifndef SCREEN_H
#define SCREEN_H

extern "C" void scroll_screen(void);
extern "C" void end_of_vga(void);
extern "C" void move_cursor(void);
extern "C" void put_char(char ch);
extern "C" void print_str(const char* s);
extern "C" void clear_screen(void);
extern "C" void print_dec(int n);
extern "C" void print_hex(unsigned int dec_n);
//extern "C" void kscanf(const char* f, ...);

#endif
