#include "common.h"
#include "idt.h"
#include "kbd.h"
#include "screen.h"
#include "time.h"
#include "page.h"

u32i infb[10];

extern "C" void cpu_vendor_info()
{
    u32i b = 0xbabef00d, c = b, d = b, a = b;
    u32i infv[4];
    asm("cpuid" : "=eax" (a), "=ebx" (b), "=ecx" (c), "=edx" (d) : "0" (0));

    infv[0] = b;
    infv[1] = d;
    infv[2] = c;
    infv[3] = 0;

    print_str("Vendor CPUID ");
    print_str((const char*)&*infv);
}

extern "C" void print_cpuid(short eax)
{
    if (eax == 1)
    {
        asm volatile("mov $0x80000002, %eax");
    }
    else if (eax == 2)
    {
        asm volatile("mov $0x80000003, %eax");
    }
    else if (eax == 3)
    {
        asm volatile("mov $0x80000004, %eax");
    }

    asm("cpuid");
    asm volatile("mov %%eax, %0" : "=r" (infb[0]));
    asm volatile("mov %%ebx, %0" : "=r" (infb[1]));
    asm volatile("mov %%ecx, %0" : "=r" (infb[2]));
    asm volatile("mov %%edx, %0" : "=r" (infb[3]));
    print_str((const char*)&infb[0]);
}

extern "C" void cpu_model()
{
    print_str("\nProcessor ");

    print_cpuid(1);
    print_cpuid(2);
    print_cpuid(3);
}

extern "C" void kmain(void)
{
    paging_init();
    IDT_init();
    kbd_init();
    timer_init();
    cpu_vendor_info();
    cpu_model();
    print_str("\nPage root dir: ");
    print_hex(get_root_dir());
    put_char('\n');
    print_str("kernel pages ");
    print_hex(get_phys_addr((unsigned int*)0x0));
    put_char('\n');
    print_hex(get_phys_addr((unsigned int*)0x3000));

    print_str("user pages ");
    print_hex(get_phys_addr((unsigned int*)(700 * 0x1000)));
    put_char(' ');
    print_hex(get_phys_addr((unsigned int*)(700 * 0x1000 + 0x1000 + 12)));
    //unsigned int* ptr = (unsigned int*)(256 * 0x1000);//0xffff;//0xffc0004;//0x00401BFF;//0x401C00;//0xA00ff;
    //unsigned int do_page_fault = *ptr;

    //unsigned int* dptr = (unsigned int*)(800*4096);//0xffff;//0xffc0004;//0x00401BFF;//0x401C00;//0xA00ff;
    //unsigned int ddo_page_fault = *dptr;
   
    while (1);
}
