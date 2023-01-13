#ifndef PREDICATES_H
#define PREDICATES_H

#include <inttypes.h>
#include <stdbool.h>
#include "middleware/data_type.h"
#include "user_interface/scanners/scanners.h"
#include "util/comparators.h"

struct Constant {
    enum DataType type;
    union {
        int64_t intVal;
        float floatVal;
        bool boolVal;
        char* stringVal;
    } value;
};

struct Condition {
    char* fieldName;
    struct Constant constant;
    enum CompareOperator operator;
    struct Condition* next;
};

struct Predicate {
    struct Condition* condition;
};

struct Condition createCondition(char* fieldName, struct Constant constant, enum CompareOperator operator);
void addCondition(struct Predicate* predicate, struct Condition condition);

#endif