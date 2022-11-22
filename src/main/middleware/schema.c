#include "string.h"
#include "data_type.h"
#include "schema.h"
#include <stdbool.h>

struct Schema* createSchema(char* name) {
    struct Schema* schema = malloc(sizeof(struct Schema));
    schema->firstField = NULL;
    schema->lastField = NULL;
    schema->slotSize = sizeof(bool);
    schema->name = name;
}

void addField(struct Schema* schema, struct String name, enum DataType type, size_t len) {
    struct Field* field = malloc(sizeof(struct Field));
    field->name = name;
    field->type = type;
    field->len = len;
    field->next = NULL;
    field->offset = schema->slotSize;
    if (schema->firstField == NULL) {
        schema->firstField = field;
    } else {
        schema->lastField->next = field;
    }
    schema->lastField = field;
    schema->slotSize += len;
}

void clearSchema(struct Schema* schema) {
    struct Field* current = schema->firstField;
    while (current != NULL) {
        struct Field* next = current->next;
        free(current);
        current = next;
    }
    schema->firstField = NULL;
    schema->lastField = NULL;
}

void addIntField(struct Schema* schema, struct String name) {
    addField(schema, name, INTEGER, sizeof(int64_t));
}

void addFloatField(struct Schema* schema, struct String name) {
    addField(schema, name, FLOAT, sizeof(float));
}

void addStringField(struct Schema* schema, struct String name, size_t len) {
    addField(schema, name, STRING, len); 
    schema->slotSize += sizeof(size_t);  // bytes containing length of the string
}

void addBooleanField(struct Schema* schema, struct String name) {
    addField(schema, name, BOOLEAN, sizeof(bool));
}

struct Field* getFieldList(struct Schema* schema) {
    return schema->firstField;
}


size_t getFieldOffset(struct Schema* schema, struct String field) {
    struct Field* current = schema->firstField;
    while (current != NULL) {
        if (compareStrings(current->name, field) == 0) {
            return current->offset;
        }
        current = current->next;
    }
}

size_t getFieldLength(struct Schema* schema, struct String field) {
    struct Field* current = schema->firstField;
    while (current != NULL) {
        if (compareStrings(current->name, field) == 0) {
            return current->len;
        }
        current = current->next;
    }
}