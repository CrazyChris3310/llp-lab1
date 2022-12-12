#ifndef READ_SCAN_H
#define READ_SCAN_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/my_string.h"

int64_t getInteger(void* scanner);
float getFloat(void* scanner);
bool getBoolean(void* scanner);
struct String getString(void* scanner);

#endif