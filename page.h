#ifndef PAGE_H
#define PAGE_H

extern "C" void fpage_handler(void);
extern "C" void* memset(void* dst, int c, int n);
extern "C" void* memcpy(void* src, void* dst, int n);
extern "C" unsigned int get_root_dir();
extern "C" unsigned int get_phys_addr(unsigned int* virt_addr);
extern "C" void paging_init(void);

#endif
