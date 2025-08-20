[BITS 32]

global paging_load_directory
global enable_paging
section .asm

paging_load_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8] ; pushes the address of the current directory
    mov cr3, eax
    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0 ; cant change cr0 directly
    or eax, 0x80000000 ; bit 31 used to enable paging
    mov cr0, eax
    pop ebp
    ret