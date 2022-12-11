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
void destroySchema(struct Schema* schema);
void clearSchema(struct Schema* schema);

void addField(struct Schema* schema, char* name, enum DataType type, size_t len);
void addIntField(struct Schema* schema, char* name);
void addFloatField(struct Schema* schema, char* name);
void addStringField(struct Schema* schema, char* name, size_t len);
void addBooleanField(struct Schema* schema, char* name);
struct Field* getFieldList(struct Schema* schema);

struct PossibleOffset getFieldOffset(struct Schema* schema, struct String field);
struct PossibleOffset getFieldLength(struct Schema* schema, struct String field);

#endif