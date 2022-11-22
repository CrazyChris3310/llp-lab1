#ifndef SCHEMA_H
#define SCHEMA_H

#include "util/my_string.h"
#include "data_type.h"
#include <stdbool.h>

struct Field {
    struct String name;
    enum DataType type;
    size_t len;
    struct Field* next;
    size_t offset;
};

struct Schema {
    char* name;
    struct Field* firstField;
    struct Field* lastField;
    size_t slotSize;
};

struct Schema* createSchema(char* name);
void clearSchema(struct Schema* schema);

void addField(struct Schema* schema, struct String name, enum DataType type, size_t len);
void addIntField(struct Schema* schema, struct String name);
void addFloatField(struct Schema* schema, struct String name);
void addStringField(struct Schema* schema, struct String name, size_t len);
void addBooleanField(struct Schema* schema, struct String name);
struct Field* getFieldList(struct Schema* schema);

size_t getFieldOffset(struct Schema* schema, struct String field);
size_t getFieldLength(struct Schema* schema, struct String field);

#endif