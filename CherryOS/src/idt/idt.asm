section .asm

global idt_load
global int21h
global no_interrupt
global enable_interrupts
global disable_interrupts

extern no_interrupt_handler
extern int21h_handler

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp
    mov ebp, esp    

    mov ebx, [ebp+8]
    lidt [ebx]
    
    pop ebp
    ret

int21h:
    cli
    pushad ; push all genereal purpose registers
    call int21h_handler 
    popad ; pop all of them
    sti
    iret

no_interrupt:
    cli
    pushad ; push all genereal purpose registers
    call no_interrupt_handler
    popad ; pop all of them
    sti
    iret
