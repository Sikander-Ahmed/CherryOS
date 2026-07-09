#include "pparser.h"
#include "string/string.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"

static int pathparser_path_valid_format(const char* filename) {
    int len = strnlen(filename, CHERRYOS_MAX_PATH);
    return (len > 0 && isdigit(filename[0]) == 0 && memcmp((void*)&filename[1], ":/", 2) == 0); // Use numbers as the filename, like 0:/, 1:/, 2:/, etc.
}

// get a drive number
static int pathparser_path_get_driver_by_path(const char** path) {
    if (!pathparser_path_valid_format(*path)) {
        return -EBADPATH;
    }

int drive_no = tonumericdigit(*path[0]);

// add 3 bytes to skip drive number
*path += 3;
return drive_no;
}

static struct path_root* pathparser_create_root(int driver_number) {
    struct path_root* path_r = kzalloc(CHERRYOS_MAX_PATH);
    path_r->drive_no = driver_number;
    path_r->first = 0;
    return path_r;
}

static const char* pathparser_get_path_part(const char** path) {
    char* result_path_part = *path;
    int i = 0;
    while (**path != 0x00 && **path != '/') {
        result_path_part[i] = **path;
        path += 1;
        i++;
    }
    if (**path == '/') { // skip the foward slash to avoid problems
        *path += 1;
    }

    if (i == 0) { // didn't parse anything
        kfree(result_path_part);
        result_path_part = 0;
    }


}

struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path) {
    const char* path_part_str = pathparser_get_path_part(path);
    if (!path_part_str) {
        return 0;
    }

    struct path_part* part = kalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;

    if (last_part) {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(struct path_root* root) {
    struct path_part* part = root->first;
    while (part) {
        struct path_part* next_part = part->next;
        kfree((void*)part->part);
        kfree(part);
        part = next_part;
    }
    kfree(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory_path) {
    int res = 0;
    const char* tmp_path = path;
    struct path_root* path_root = 0;
    if (strlen(path) > CHERRYOS_MAX_PATH) {
        goto out;
    }

    res = pathparser_get_driver_by_path(&tmp_path);
    if (res < 0) {
        goto out;
    }

    path_root = pathparser_create_root(res);
    if (!path_root) {
        goto out;
    }

    struct path_part* first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if (!first_part) {
        goto out;
    }
    
    path_root->first = first_part;
    struct path_part* part = pathparser_parse_path_part(first_part, &tmp_path);
    while (part) {
        part = pathparser_parse_path_part(part, &tmp_path);
    }



    
out:
    
}