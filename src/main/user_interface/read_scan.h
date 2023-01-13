#ifndef READ_SCAN_H
#define READ_SCAN_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/my_string.h"
#include "user_interface/predicates.h"

struct ScanInterface;

bool next(struct ScanInterface* scanner);

int64_t getInt(struct ScanInterface* scanner, char* field);
float getFloat(struct ScanInterface* scanner, char* field);
bool getBool(struct ScanInterface* scanner, char* field);
struct String getString(struct ScanInterface* scanner, char* field);
struct Constant getField(struct ScanInterface* scanner, char* field);

// destroy given scanners and all inner scanners
void destroyScanner(struct ScanInterface* scanner);

#endif