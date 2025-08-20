#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"

void paging_load_directory(uint32_t* directory);
static uint32_t* current_directory = 0;



struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
    int offset = 0;
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE); // sets everything to 0 for each page
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags; // gives the physical address, starts with 0
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE); // goes past the full table, an
        directory[i] = (uint32_t) entry | flags | PAGING_IS_WRITABLE;
    }

    struct paging_4gb_chunk* chunck_4gb = kzalloc(sizeof(struct paging_4gb_chunk)); // creats new chunk pointer 
    chunck_4gb->directory_entry = directory;
    
    return chunck_4gb;
}

uint32_t* paging_4gb_chunk_get_direcotry(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}

void paging_switch(uint32_t* directory) {
    paging_load_directory(directory);
    current_directory = directory;
}


bool paging_is_aligned(void* address) {
    return ((uint32_t) address % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out) {

    int res = 0;

    if (!paging_is_aligned(virtual_address)) {
    res = -EINVARG;
    goto out; 
    }

    // 1024 * 4096 will give us the directory index. the whole thing
    *directory_index_out = ((uint32_t) virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)); 
    // this will give us the index we are looking for, given the address
    *table_index_out = ((uint32_t) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) PAGING_PAGE_SIZE); 

out: // we use goto to clean out the memory
    return res;
}

int paging_set(uint32_t* directory, void* virtual_address, uint32_t val) { // val is the page table entry
    if (!paging_is_aligned(virtual_address)) {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index;
    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if (res < 0) {
        return res;
    }

    uint32_t entry = directory[directory_index]; // get the page table entry
    uint32_t* table = (uint32_t*)(entry &0xFFFFF000); // get just the address
    table[table_index] = val;
    
}
