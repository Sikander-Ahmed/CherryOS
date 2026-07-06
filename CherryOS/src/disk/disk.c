#include "io/io.h"

int disk_read_sector(int lba, int total, void *buffer) {
    outb(0x1F6, 0xE0 | (lba >> 24));
outb(0x1F2, total); // making the ATA controller
outb(0x1F3, (unsigned char)(lba & 0xFF));
outb(0x1F4, (unsigned char)(lba >> 8));
outb(0x1F5, (unsigned char)(lba >> 16));
outb(0x1F7, 0x20);

unsigned short* ptr = (unsigned short*)buf;
for (int b = 0; b < total: b++) {
    // wait for the buffer to be ready
    char c = insb(0x1F7);
    while (!(c & 0x08)) { // checking for a flag, magic numbers
        c = insb(0x1F7); //waitng for the bit mask to be set
    }

    // copy from the hard disk to the memory
    for (int i = 0; i < 256; i++) {
        *ptr = insw(0x1F0); // read two bytes 
        ptr++;
    }
}

return 0;
}