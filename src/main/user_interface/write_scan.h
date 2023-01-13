#ifndef WRITE_SCAN_H
#define WRITE_SCAN_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/my_string.h"

struct ScanInterface;

void insert(struct ScanInterface* scanner);
void deleteRecord(struct ScanInterface* scanner);

void setInt(struct ScanInterface* scanner, char* field, int64_t value);
void setFloat(struct ScanInterface* scanner, char* field, float value);
void setBool(struct ScanInterface* scanner, char* field, bool value);
void setString(struct ScanInterface* scanner, char* field, struct String value);
void setVarchar(struct ScanInterface* scanner, char* field, char* value);
void setField(struct ScanInterface* scanner, char* field, struct Constant value);

#endif