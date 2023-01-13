#include "user_interface/predicates.h"
#include <stdlib.h>

struct Condition createCondition(char* fieldName, struct Constant constant, enum CompareOperator operator) {
    return (struct Condition){ .fieldName = fieldName, .constant = constant, .operator = operator};
}

void addCondition(struct Predicate* predicate, struct Condition condition) {
    addNode(predicate->conditions, &condition);
}

struct Predicate* createPredicate() {
    struct Predicate* predicate = malloc(sizeof(struct Predicate));
    predicate->conditions = createLinkedList();
    return predicate;
}

void destroyPredicate(struct Predicate* predicate) {
    freeLinkedList(predicate->conditions);
    free(predicate);
}