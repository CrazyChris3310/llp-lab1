#include "string.h"
#include "data_type.h"
#include "schema.h"
#include <stdlib.h>
#include <stdbool.h>
#include "file_io/page.h"

struct Schema* createSchema(char* name) {
    struct Schema* schema = malloc(sizeof(struct Schema));
    schema->firstField = NULL;
    schema->lastField = NULL;
    schema->slotSize = sizeof(bool);
    schema->name = name;
    return schema;
}

void destroySchema(struct Schema* schema) {
    clearSchema(schema);
    free(schema);
}

// FIXME:
void addField(struct Schema* schema, char* name, enum DataType type, size_t len) {
    struct Field* field = malloc(sizeof(struct Field));
    size_t length = strlen(name);
    char* realName = malloc(length + 1);
    memcpy(realName, name, length);
    realName[length] = '\0';
    field->name = (struct String){.value = realName, .lenght = length };
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

// FIXME:
void clearSchema(struct Schema* schema) {
    struct Field* current = schema->firstField;
    while (current != NULL) {
        struct Field* next = current->next;
        free(current->name.value);
        free(current);
        current = next;
    }
    schema->firstField = NULL;
    schema->lastField = NULL;
}

void addIntField(struct Schema* schema, char* name) {
    addField(schema, name, INTEGER, sizeof(int64_t));
}

void addFloatField(struct Schema* schema, char* name) {
    addField(schema, name, FLOAT, sizeof(float));
}

size_t bitsToBytes(size_t bits) {
    return bits / 8 + 1;
}

void addStringField(struct Schema* schema, char* name, size_t len) {
    addField(schema, name, STRING, len + 1); 
    schema->slotSize += sizeof(size_t);  // bytes containing length of the string
}

void addBooleanField(struct Schema* schema, char* name) {
    addField(schema, name, BOOLEAN, sizeof(bool));
}

struct Field* getFieldList(struct Schema* schema) {
    return schema->firstField;
}


struct PossibleOffset getFieldOffset(struct Schema* schema, struct String field) {
    struct Field* current = schema->firstField;
    while (current != NULL) {
        if (compareStrings(current->name, field) == 0) {
            return (struct PossibleOffset){ .exists=true, .offset=current->offset };
        }
        current = current->next;
    }
    return (struct PossibleOffset){ .exists=false };
}

struct PossibleOffset getFieldLength(struct Schema* schema, struct String field) {
    struct Field* current = schema->firstField;
    while (current != NULL) {
        if (compareStrings(current->name, field) == 0) {
            return (struct PossibleOffset){ .exists=true, .offset=current->len };
        }
        current = current->next;
    }
    return (struct PossibleOffset){ .exists=false };
}