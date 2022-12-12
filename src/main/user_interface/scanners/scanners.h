#ifndef SCANNERS_H
#define SCANNERS_H

#include <inttypes.h>
#include <stdbool.h>
#include "middleware/schema.h"
#include "page_cacheing/cache_manager.h"

struct TableScanner;

bool next(struct TableScanner* scanner);
void insert(struct TableScanner* scanner);

struct TableScanner* createTableScanner(struct CacheManager* cm, struct Schema* schema, bool isNew, size_t blockStart);
void destroyTableScanner(struct TableScanner* scanner);

size_t getCurrentBlock(struct TableScanner* scanner);

int64_t getInt(struct TableScanner* scanner, char* field);
float getFloat(struct TableScanner* scanner, char* field);
bool getBool(struct TableScanner* scanner, char* field);
struct String getString(struct TableScanner* scanner, char* field);


void setInt(struct TableScanner* scanner, char* field, int64_t value);
void setFloat(struct TableScanner* scanner, char* field, float value);
void setBool(struct TableScanner* scanner, char* field, bool value);
void setString(struct TableScanner* scanner, char* field, struct String value);
void setVarchar(struct TableScanner* scanner, char* field, char* value);

#endif