section ._start
bits 16
global _start
_start:
    mov ax, 0x2401
    int 0x15 ; enable A20 bit

    mov ax, 0x3
    int 0x10 ; set vga text mode 3

    mov [disk], dl

    mov ah, 0x2  ;read sectors
    mov al, 6  ;sectors to read
    mov ch, 0  ;cylinder idx
    mov dh, 0  ;head disk
    mov cl, 2  ;sector idx
    mov dl, [disk]  ;disk idx
    mov bx, copy_target  ;target_pointer
    int 0x13
    cli
    lgdt [gdt_pointer] ; load the gdt table
    mov eax, cr0 
    or eax, 0x1 ; set the protected mode bit on special CPU reg cr0
    mov cr0, eax
    jmp CODE_SEG : boot2 ; long jump to the code segment

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
gdt_pointer:
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

STACK_SIZE equ 16384

global load_idt
global kbd_handler
global kernel

extern kmain
extern print_handler

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti 
	ret
	
page_isr:
	call page_handler
	iretd

kbd_isr:  
	call kbd_handler
	iretd
	
pit_isr:
	call pit_handler
	iretd
	
get_eip:
	mov eax, [esp]
	ret

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
    hlt

section .bss
res_stack:
    align 4
    stack_bottom: equ $
    resb STACK_SIZE
    stack_top:
