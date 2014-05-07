#include "string.h"

void* memset(void* ptr, int value, size_t num) {
    unsigned char* data = (unsigned char*)ptr;
    for(size_t i = 0; i < num; i++) {
        data[i] = (unsigned char)value;
    }
    return ptr;
}

int strcmp(const char* str1, const char* str2) {
    while(*str1 && (*str1==*str2))
        str1++,str2++;
    return *(const unsigned char*)str1-*(const unsigned char*)str2;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}
