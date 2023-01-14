#include "user_interface/predicates.h"
#include <stdlib.h>

struct Condition* createCondition(char* fieldName, struct Constant constant, enum CompareOperator operator) {
    struct Condition* condition = malloc(sizeof(struct Condition));
    condition->fieldName = fieldName;
    condition->constant = constant;
    condition->operator = operator;
    return condition;
}

void addCondition(struct Predicate* predicate, char* fieldName, struct Constant constant, enum CompareOperator operator) {
    struct Condition* condition = createCondition(fieldName, constant, operator);
    addNode(predicate->conditions, condition);
}

struct Predicate* createPredicate() {
    struct Predicate* predicate = malloc(sizeof(struct Predicate));
    predicate->conditions = createClearableLinkedList(free);
    return predicate;
}

void destroyPredicate(struct Predicate* predicate) {
    if (predicate == NULL) {
        return;
    }
    freeLinkedList(predicate->conditions);
    free(predicate);
}

struct Constant intConstant(int64_t value) {
    return (struct Constant){ .type = INT, .value.intVal = value };
}

struct Constant floatConstant(float value) {
    return (struct Constant){ .type = FLOAT, .value.floatVal = value };
}

struct Constant boolConstant(bool value) {
    return (struct Constant){ .type = BOOL, .value.boolVal = value };
}

struct Constant stringConstant(char* value) {
    return (struct Constant){ .type = STRING, .value.stringVal = value };
}

