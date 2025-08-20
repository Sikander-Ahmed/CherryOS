#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"

uint16_t* video_mem = 0; // This address is used for outputting text to the screen, 1 byte for the char, and another for the color
uint16_t  terminal_row, terminal_col = 0;
static struct paging_4gb_chunk* kernel_chunk = 0;


uint16_t terminal_make_char(char c, char color) {
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color) {
    
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
} 

void terminal_initialize() {
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) {
        len++;
    }
    return len;
}

void print(const char* str) {
    size_t len = strlen(str);

    for (int i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }

}


void kernel_main() {
    terminal_initialize();
    print("Hello World!\nYOOOOOOOOOOOO");

    kheap_init(); // initalize the heap

    idt_init(); // initialze the global descriptor table

    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL); // setup paging
   
    // 4096 bytes to memeory, andd dset 0x1000 to it
    char* ptr = kzalloc(4096);

    // put the physical address and map it to a virtual address, and will not affect 0x1000
    paging_set(paging_4gb_chunk_get_direcotry(kernel_chunk), (void*) 0x1000, (uint32_t) ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);

    paging_switch(paging_4gb_chunk_get_direcotry(kernel_chunk)); // switch to kernel paging chunk

    enable_paging(); // enable paging

    enable_interrupts(); // enable system interrupts
    

}