#include "user_interface/query.h"
#include <stdlib.h>

struct SelectQuery* createSelectQuery(char* from, struct Predicate* predicate) {
    struct SelectQuery* query = malloc(sizeof(struct SelectQuery));
    query->from = from;
    query->predicate = predicate;
    return query;
}

void destroySelectQuery(struct SelectQuery* query) {
    if (query == NULL) {
        return;
    }
    destroyPredicate(query->predicate);
    free(query);
}

/*
    * Creates an insert query with the given table name and values.
    * 
    * @param into The name of the table to insert into.
    * @param values List of Condition structs containing pair of column name and value.
    * @return The insert query.
*/
struct InsertQuery* createInsertQuery(char* into) {
    struct InsertQuery* query = malloc(sizeof(struct InsertQuery));
    query->into = into;
    query->values = createLinkedList();
    return query;
}

void clearInsertQuery(struct InsertQuery* query) {
    if (query != NULL) {
        clearList(query->values);
    }
}

void destroyInsertQuery(struct InsertQuery* query) {
    freeLinkedList(query->values);
    free(query);
}

void addInsertionField(struct InsertQuery* query, char* field, struct Constant value) {
    struct Condition* condition = createCondition(field, value, EQUAL);
    addNode(query->values, condition);
}

struct DeleteQuery* createDeleteQuery(char* from, struct Predicate* predicate) {
    struct DeleteQuery* query = malloc(sizeof(struct DeleteQuery));
    query->from = from;
    query->predicate = predicate;
    return query;
}

void destroyDeleteQuery(struct DeleteQuery* query) {
    destroyPredicate(query->predicate);
    free(query);
}

struct UpdateQuery* createUpdateQuery(char* table, char* field, struct Constant* value, struct Predicate* predicate) {
    struct UpdateQuery* query = malloc(sizeof(struct UpdateQuery));
    query->table = table;
    query->condition = createCondition(field, *value, EQUAL);
    query->predicate = predicate;
    return query;
}
