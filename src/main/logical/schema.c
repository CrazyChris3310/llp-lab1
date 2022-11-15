#include "string.h"
#include "data_type.h"
#include <stdbool.h>

struct Field {
    struct String name;
    enum DataType type;
    size_t len;
    struct Field* next;
};

struct Schema {
    struct Field* firstField;
    struct Field* lastField;
};

void addField(struct Schema* schema, struct String name, enum DataType type, size_t len) {
    struct Field* field = malloc(sizeof(struct Field));
    field->name = name;
    field->type = type;
    field->len = len;
    field->next = NULL;
    if (schema->firstField == NULL) {
        schema->firstField = field;
    } else {
        schema->lastField->next = field;
    }
    schema->lastField = field;
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
}

void addBooleanField(struct Schema* schema, struct String name) {
    addField(schema, name, BOOLEAN, sizeof(bool));
}
