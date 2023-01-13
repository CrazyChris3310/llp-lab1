#include "user_interface/query.h"

struct SelectQuery createSelectQuery(char* from, struct Predicate* predicate) {
    return (struct SelectQuery) {
        .from = from,
        .predicate = predicate
    };
}

struct InsertQuery createInsertQuery(char* into, struct LinkedList* values) {
    return (struct InsertQuery) {
        .into = into,
        .values = values
    };
}

struct DeleteQuery createDeleteQuery(char* from, struct Predicate* predicate) {
    return (struct DeleteQuery) {
        .from = from,
        .predicate = predicate
    };
}

struct UpdateQuery createUpdateQuery(char* table, char* field, struct Constant* value, struct Predicate* predicate) {
    return (struct UpdateQuery) {
        .table = table,
        .field = field,
        .value = value,
        .predicate = predicate
    };
}