#include "string.h"


int strlen(const char* ptr) {
    int i = 0;
    while(*ptr) {
        i++;
        ptr+= 1;   
    }
    return i;
}

int strnlen(const char* ptr, int max) {
    int i = 0;
    for (i = 0; i < max; i++){
        if (ptr[i] == 0) {
            break;;
        }
    }
    
    return i;
}

int tonumericdigit(char c) {
    return c - 48;
}

int isdigit(char c) {
    return c >= 48 && c <= 57;
}