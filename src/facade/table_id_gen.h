
#ifndef TABLE_ID_GEN_H
#define TABLE_ID_GEN_H

#include <inttypes.h>

size_t count = 0;

size_t get_next_id() {
    return count++;
}

#endif