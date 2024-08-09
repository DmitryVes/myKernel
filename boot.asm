section ._start
bits 16
global _start
_start:
    mov ax, 0x2401
    int 0x15 

    mov ax, 0x3
    int 0x10

    mov [disk], dl

    ;head0, head1
    ;head0 = head1 = 80 tracks
    ;1 track = 18 sectors
    ;sector = 512 байт

    mov ah, 0x2 ;read sectors
    mov al, 24 ;sectors to read
    mov ch, 0  ;cylinder idx
    mov dh, 0  ;head disk
    mov cl, 2  ;sector idx
    mov dl, [disk]  ;disk idx
    mov bx, copy_target  
    int 0x13

    cli
    lgdt [gdt_ptr] 
    mov eax, cr0 
    or eax, 0x1 
    mov cr0, eax
    jmp CODE_SEG : boot2

gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:    
gdt_ptr:
    dw gdt_end - gdt_start
    dd gdt_start

disk:
    db 0x0

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510 - ($-$$) db 0
dw 0xaa55

copy_target:

bits 32

KERNEL_STACK_SIZE equ 16384

global load_idt

global kbd_isr
global timer_isr
global pg_isr
global kernel

extern kmain
extern print_handler
extern timer_handler
extern fpage_handler

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti 
	ret

pg_isr:
    call fpage_handler
    iretd

timer_isr:
    call timer_handler
    iretd

kbd_isr:  
	call print_handler
	iretd

boot2:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

kernel:
    mov esp, kernel_stack_top
    call kmain

section .bss
align 4
kernel_stack_bottom: equ $
    resb KERNEL_STACK_SIZE
kernel_stack_top:
