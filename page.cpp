#include "page.h"
#include "common.h"
#include "screen.h"

unsigned int page_dir[1024] __attribute__((aligned(4096)));
unsigned int page_table[1024] __attribute__((aligned(4096)));

unsigned int* frame_set;

/*
* адресуемая память (оперативка процессора) - 0x800000 (8 мб),
* используется одна таблица страниц на 512 страниц, - 
* страницами покрыто 2 мб памяти.
*/

extern "C" void init_frame_set(void)
{
     memset((void*)frame_set, 0, 128);
}

extern "C" void set_frame(int n)
{
    *(frame_set + n / 32) |= 1 << (n % 32);
}

extern "C" void clear_frame(int n)
{
    *(frame_set + n / 32) &= ~(1 << (n % 32));
}

extern "C" int find_free_frame()
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 32; j++) {
            if (((*(frame_set + i)) & (1 << j)) == 0) {
                return i * 32 + j;
            }
        }
    }
    return -1;
}

extern "C" void* memset(void* dst, int c, int n);
extern "C" void frame_alloc(unsigned int* virt_addr);
extern "C" void frame_dealloc(unsigned int* virt_addr);

extern "C" void switch_pg_dir(unsigned int pd_addr)
{
    asm volatile("mov %0, %%eax\n\t" : "=r" (pd_addr));
    asm volatile("mov %eax, %cr3\n\t");
}

extern "C" void paging_init(void)
{
    /*
    * очистка и заполнение полей адресов таблиц страниц.
    */

    init_frame_set();

    for (int i = 0; i < 1024; i++)
    {
        page_dir[i] = 0x2;
    }
    
    for (unsigned int i = 0; i < 512; i++)
    {
        page_table[i] = (i * 0x1000) | 1;
        set_frame(i);
    }

    /*
    * инициализация каталога страниц и переход в страничный режим.
    */

    page_dir[0] = ((unsigned int)page_table) | 3;
    switch_pg_dir((unsigned int)page_dir);
    asm volatile("mov %cr0, %eax\n\t");
    asm volatile("or $0x80000001, %eax\n\t");
    asm volatile("mov %eax, %cr0\n\t");

    print_dec(*(frame_set+1));
}

extern "C" void fpage_handler(void)
{
    unsigned int err_code;
    asm volatile("mov %0, %%eax" : "=r" (err_code));
    print_hex(err_code);
    u32i itp_addr;
    asm volatile("mov %%cr2, %0" : "=r" (itp_addr));
    print_str("\npage fault! (");
    if (!(err_code & 0x1)) { print_str(" present "); }
    if (err_code & 0x2) print_str(" read-only ");
    if (err_code & 0x4) print_str(" user-mode ");
    if (err_code & 0x8) print_str(" reserved ");
    print_str(") interrupted in ");
    print_hex(itp_addr);
    put_char('\n');
    frame_alloc((unsigned int*)itp_addr);
    print_str("alloc new userframe ");
    outb(0x20, 0x20);
}

extern "C" void* memset(void* dst, int c, int n)
{
    if (dst != NULL)
    {
        char* tmp_dst = (char*)dst;
        while (n--)
        {
            *tmp_dst++ = c;
        }
    }
    return dst;
}

extern "C" void* memcpy(void* src, void* dst, int n)
{
    char* tmp_src = (char*)src;
    char* tmp_dst = (char*)dst;

    while (n--) *tmp_dst++ = *tmp_src++;
    return dst;
}

//extern "C" void tlb_flush()

extern "C" unsigned int get_root_dir()
{
    /*
    * адрес каталога страниц
    */

    unsigned int pg_root_dir;
    asm volatile("movl %%cr3, %0" : "=r" (pg_root_dir));
    return pg_root_dir;
}

extern "C" unsigned int get_phys_addr(unsigned int* virt_addr)
{
    unsigned int pg_dir_idx, pg_table_idx;

    pg_dir_idx = (unsigned int)virt_addr >> 22;

    pg_table_idx = (unsigned int)virt_addr >> 12 & 0x03FF;

    unsigned int pt_addr = ((unsigned int*)(get_root_dir()))[pg_dir_idx] & ~0xFFF;
    unsigned int p_addr = ((unsigned int*)pt_addr)[pg_table_idx] & ~0xFFF;
    return p_addr + ((unsigned int)virt_addr & 0xFFF);
}

extern "C" void frame_alloc(unsigned int* virt_addr)
{
    /*
    * выделение страницы (установка бита выделения),
    её очистка (заполнение всех байтов нулями)
    */

    page_table[(unsigned int)virt_addr >> 12 & 0x03FF] = (((unsigned int)virt_addr >> 12 & 0x03FF) * 0x1000) | 3;
    unsigned int phys_ad = get_phys_addr((unsigned int*)(virt_addr));
    memset((void*)(phys_ad - phys_ad % 0x1000), 0, 0x1000);
    set_frame(find_free_frame());
}

extern "C" void frame_dealloc(unsigned int* virt_addr)
{
    /*
    * освобождение страницы (снятие бита выделения страницы)
    */

    page_table[(unsigned int)virt_addr >> 12 & 0x03FF] = (((unsigned int)virt_addr >> 12 & 0x03FF) * 0x1000) & 0;
    //clear_frame();
}
