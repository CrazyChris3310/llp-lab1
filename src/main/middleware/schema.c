#include "string.h"
#include "data_type.h"
#include "schema.h"
#include <stdlib.h>
#include <stdbool.h>
#include "file_io/page.h"

static void freeField(void* ptr) {
    struct Field* field = (struct Field*)ptr;
    free(field->name.value);
    free(field);
}

struct Schema* createSchema(char* name) {
    struct Schema* schema = malloc(sizeof(struct Schema));
    schema->fields = createClearableLinkedList(freeField);
    schema->slotSize = sizeof(bool);
    schema->name = name;
    schema->shouldClear = false;
    return schema;
}

void destroySchema(struct Schema* schema) {
    clearSchema(schema);
    freeLinkedList(schema->fields);
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
    field->offset = schema->slotSize;
    addNode(schema->fields, field);
    schema->slotSize += len;
    if (type == STRING) {
        schema->slotSize += sizeof(size_t);
    }
}

// FIXME:
void clearSchema(struct Schema* schema) {
    if (schema == NULL) {
        return;
    }
    clearList(schema->fields);
}

void addIntField(struct Schema* schema, char* name) {
    addField(schema, name, INT, sizeof(int64_t));
}

void addFloatField(struct Schema* schema, char* name) {
    addField(schema, name, FLOAT, sizeof(float));
}

void addStringField(struct Schema* schema, char* name, size_t len) {
    addField(schema, name, STRING, len + 1); 
}

void addBooleanField(struct Schema* schema, char* name) {
    addField(schema, name, BOOL, sizeof(bool));
}

struct LinkedList* getFieldList(struct Schema* schema) {
    return schema->fields;
}

struct PossibleOffset getFieldOffset(struct Schema* schema, struct String field) {
    struct PossibleOffset result = { .exists=false };
    struct ListIterator* iterator = createListIterator(schema->fields);
    while (iteratorHasNext(iterator)) {
        struct Field* current = (struct Field*)iteratorNext(iterator);
        if (compareStrings(current->name, field) == 0) {
            result = (struct PossibleOffset){ .exists=true, .offset=current->offset };
            break;
        }
    }
    freeListIterator(iterator);
    return result;
}

struct PossibleOffset getFieldLength(struct Schema* schema, struct String field) {
    struct PossibleOffset result = { .exists=false };
    struct ListIterator* iterator = createListIterator(schema->fields);
    while (iteratorHasNext(iterator)) {
        struct Field* current = (struct Field*)iteratorNext(iterator);
        if (compareStrings(current->name, field) == 0) {
            result = (struct PossibleOffset){ .exists=true, .offset=current->len };
            break;
        }
    }
    freeListIterator(iterator);
    return result;
}

enum DataType getFieldType(struct Schema* schema, struct String field) {
    enum DataType result = -1;
    struct ListIterator* iterator = createListIterator(schema->fields);
    while (iteratorHasNext(iterator)) {
        struct Field* current = (struct Field*)iteratorNext(iterator);
        if (compareStrings(current->name, field) == 0) {
            result = current->type;
            break;
        }
    }
    freeListIterator(iterator);
    return result;
}