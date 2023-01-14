#ifndef QUERY_H
#define QUERY_H

#include "util/linked_list.h"
#include "user_interface/predicates.h"

struct SelectQuery {
    char* from;
    struct Predicate* predicate;  
};

struct InsertQuery {
    char* into;
    struct LinkedList* values;
};

struct DeleteQuery {
    char* from;
    struct Predicate* predicate;
};

struct UpdateQuery {
    char* table;
    struct Condition* condition;
    struct Predicate* predicate;
};

struct SelectQuery* createSelectQuery(char* from, struct Predicate* predicate);
struct InsertQuery* createInsertQuery(char* into);
struct DeleteQuery* createDeleteQuery(char* from, struct Predicate* predicate);
struct UpdateQuery* createUpdateQuery(char* table, char* field, struct Constant* value, struct Predicate* predicate);

void destroySelectQuery(struct SelectQuery* query);
void destroyInsertQuery(struct InsertQuery* query);
void destroyDeleteQuery(struct DeleteQuery* query);
void destroyUpdateQuery(struct UpdateQuery* query);

void addInsertionField(struct InsertQuery* query, char* field, struct Constant value);

#endif