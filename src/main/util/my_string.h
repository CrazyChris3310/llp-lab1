#ifndef MY_STRING_H
#define MY_STRING_H

#include <inttypes.h>
#include <string.h>

struct String {
    size_t lenght;
    char* value;
};

int compareStrings(struct String a, struct String b);
int equals(struct String a, struct String b);

#endif