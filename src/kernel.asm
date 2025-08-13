[BITS 32]

global _start
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000 ; 4 gb?
    mov esp, ebp ; setting stack pointer

    ; enable the a20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; remap the master PIC
    mov al, 00010001b ; intialization mode
    out 0x20, al ; Tell master PIC
    mov al, 0x20 ; interrupt 0x20 is where master ISR is should start
    out 0x21, al

    mov al, 00000001b ; put it in x86 mode
    out 0x21, al
    ; end remap of master PIC
    
    sti

    call kernel_main
    jmp $


times 512-($ - $$) db 0 