#ifndef MY_STRING_H
#define MY_STRING_H

#include <inttypes.h>

struct String {
    size_t lenght;
    char* value;
};

void copyString(struct String src, void** dest) {
    *(int64_t*)*dest = src.lenght;
    char* str_dest = (char*)*dest + sizeof(int64_t);
    for (size_t i = 0; i < src.lenght; ++i) {
        str_dest[i] = src.value[i];
    }
}

#endif