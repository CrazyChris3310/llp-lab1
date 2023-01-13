#include "user_interface/predicates.h"

struct Condition createCondition(char* fieldName, struct Constant constant, enum CompareOperator operator) {
    return (struct Condition){ .fieldName = fieldName, .constant = constant, .operator = operator};
}

void addCondition(struct Predicate* predicate, struct Condition condition) {
    condition.next = NULL;
    struct Condition* current = predicate->condition;
    if (current == NULL) {
        predicate->condition = &condition;
        return;
    }
    while (current->next != NULL) {
        current = current->next;
    };
    current->next = &condition;
}
