#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"

struct idt_desc idt_descriptors[CHERRYOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();


void int21h_handler() {
    print("Keyboard Pressed");
    outb(0x20, 0x20);
}

void no_interrupt_handler() {
    outb(0x20, 0x20);
}

void idt_zero() {
    print("Divide by zero error\n");
}

void idt_set(int interrupt_no, void* address) {
   struct idt_desc* desc = &idt_descriptors[interrupt_no];
   desc->offset_1 = (uint32_t) address & 0x0000FFFF; // set up offest 1
   desc->selector = KERNEL_CODE_SELECTOR;
   desc->zero = 0x00;
   desc->type_attr = 0xEE; // settuping other bits up as well
   desc->offset_2 = (uint32_t) address >> 16; // shifting 16 bits because of higher bits
}


void idt_init() {
    memset(idt_descriptors, 0, sizeof(idt_descriptors)); // create null descirptros
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1; // limit is -1
    idtr_descriptor.base = (uint32_t) idt_descriptors;

    for (int i = 0; i < CHERRYOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, no_interrupt); // This will set all the interrupts to do nothing for now
    }

    idt_set(0, idt_zero); // divide by zero interrupt
    idt_set(0x21, int21h); // keyboard press interrupt

    // load interrupt desciptor table
    idt_load(&idtr_descriptor);
}
