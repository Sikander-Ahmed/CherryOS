#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// refer to osdev paging table
#define PAGING_CACHE_DISABLED  0b00010000 // disable or enable cache
#define PAGING_WRITE_THROUGH   0b00001000 // enable write through
#define PAGING_ACCESS_FROM_ALL 0b00000100 // supervisor or user space can use (not all processes!)
#define PAGING_IS_WRITABLE     0b00000010 // page is write and read (not read only)
#define PAGING_IS_PRESENT      0b00000001 // page faults

#define PAGING_PAGE_SIZE 4096
#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024 //

struct paging_4gb_chunk {
    uint32_t* directory_entry;
    
};

void paging_switch(uint32_t* directory);
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void enable_paging();
uint32_t* paging_4gb_chunk_get_direcotry(struct paging_4gb_chunk* chunk);
int paging_set(uint32_t* directory, void* virtual_address, uint32_t val);
bool paging_is_aligned(void* address);
int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out);


#endif