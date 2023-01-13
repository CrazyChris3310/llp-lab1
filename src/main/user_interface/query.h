#ifndef QUERY_H
#define QUERY_H

#include "util/linked_list.h"

struct Predicate;
struct Constant;

struct SelectQuery {
    char* from;
    struct Predicate* predicate;  
};

struct InsertQuery {
    char* into;
    struct LinkedList* fields;
    struct Constant* values;
};

struct DeleteQuery {
    char* from;
    struct Predicate* predicate;
};

struct UpdateQuery {
    char* table;
    char* field;
    struct Constant* value;
    struct Predicate* predicate;
};

struct SelectQuery createSelectQuery(char* from, struct Predicate* predicate);
struct InsertQuery createInsertQuery(char* into, struct LinkedList* fields, struct Constant* values);
struct DeleteQuery createDeleteQuery(char* from, struct Predicate* predicate);
struct UpdateQuery createUpdateQuery(char* table, char* field, struct Constant* value, struct Predicate* predicate);

#endif