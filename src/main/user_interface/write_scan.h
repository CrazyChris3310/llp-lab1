#ifndef READ_SCAN_H
#define READ_SCAN_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/my_string.h"

void setInteger(void* scanner, int64_t value);
void setFloat(void* scanner, float value);
void setBoolean(void* scanner, bool value);
void setString(void* scanner, char* value);
void setVarchar(void* scanner, struct String value);

#endif