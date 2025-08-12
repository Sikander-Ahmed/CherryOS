#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_desc { // check interrupt descitor table on os.dev to see what each bit means!
    uint16_t offset_1; // offset bits 0-15
    uint16_t selector; // selector that is in our GDT
    uint8_t zero; // does nothing
    uint8_t type_attr; // descriptor type and attributes
    uint16_t offset_2; // offset bits 16-31
} __attribute__((packed));

struct idtr_desc {
    uint16_t limit; // size of descriptor table
    uint32_t base; // base address of the start of interrupt descriptor table
} __attribute__((packed));

void idt_init();

#endif