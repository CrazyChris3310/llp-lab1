#include "middleware/schema.h"
#include "user_interface/read_scan.h"
#include "user_interface/scanners/scanners.h"
#include "user_interface/predicates.h"
#include "user_interface/database.h"
#include "user_interface/query.h"
#include "util/linked_list.h"
#include <assert.h>


void testCreateTable() {
    printf("Start create tables test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    
    struct Schema* found = findTable(database, "test");
    struct LinkedList* expected = getFieldList(schema);
    struct LinkedList* received = getFieldList(found);

    assert(getListSize(expected) == getListSize(received));

    struct ListIterator* expectedIterator = createListIterator(expected);
    struct ListIterator* receivedIterator = createListIterator(received);

    while (iteratorHasNext(expectedIterator)) {
        struct Field* expectedField = (struct Field*)iteratorNext(expectedIterator);
        struct Field* receivedField = (struct Field*)iteratorNext(receivedIterator);

        assert(equals(expectedField->name, receivedField->name));
        assert(expectedField->type == receivedField->type);
        assert(expectedField->len == receivedField->len);
        assert(expectedField->offset == receivedField->offset);
    }

    freeListIterator(expectedIterator);
    freeListIterator(receivedIterator);

    destroySchema(schema);
    destroySchema(found);

    closeDatabase(database);
    printf("Teset successfully finished\n\n");
}

void testInsertAndReadData() {
    printf("Starting isertion test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");
    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(12.5f));
    addInsertionField(query, "number", constant(123));

    performInsertQuery(database, query);
    destroyInsertQuery(query);
    
    struct SelectQuery* selectQuery = createSelectQuery("test", NULL);
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);

    next(scanner);
    assert(getInt(scanner, "number") == 123);
    assert(compareFloats(getFloat(scanner, "floating"), 12.5f, EQUAL));
    assert(strcmp(getString(scanner, "name").value, "abuabu") == 0);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);
    
    printf("Test successfully finished\n\n");
}

void testMultipleInsert() {
    printf("Starting muiltiple insert test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");
    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(12.5f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);
    destroyInsertQuery(query);

    query = createInsertQuery("test");
    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(-5));
    performInsertQuery(database, query);
    destroyInsertQuery(query);
    
    struct SelectQuery* selectQuery = createSelectQuery("test", NULL);
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);

    assert(next(scanner));
    assert(getInt(scanner, "number") == 123);
    assert(compareFloats(getFloat(scanner, "floating"), 12.5f, EQUAL));
    assert(strcmp(getString(scanner, "name").value, "abuabu") == 0);

    assert(next(scanner));
    assert(getInt(scanner, "number") == -5);
    assert(compareFloats(getFloat(scanner, "floating"), 1246.2f, EQUAL));
    assert(strcmp(getString(scanner, "name").value, "abuabu") == 0);

    assert(next(scanner) == false);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);
    
    printf("Test successfully finished\n\n");
}

void testSelectWithCondition() {
    printf("Starting conditional select test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");

    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(12.5f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(-5));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("asdf"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(182341));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("qwer"));
    addInsertionField(query, "floating", constant(-47.214f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    destroyInsertQuery(query);

    struct Predicate* predicate = createPredicate();
    addCondition(predicate, "name", constant("abuabu"), EQUAL);
    
    struct SelectQuery* selectQuery = createSelectQuery("test", predicate);
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);

    int count = 0;
    while (next(scanner)) {
        assert(strcmp(getString(scanner, "name").value, "abuabu") == 0);
        count++;
    }
    assert(count == 2);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);
    
    printf("Test successfully finished\n\n");
}

void testDeleteData() {
    printf("Starting delete test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");

    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(12.5f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(-5));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("asdf"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(182341));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("qwer"));
    addInsertionField(query, "floating", constant(-47.214f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    destroyInsertQuery(query);

    struct SelectQuery* selectQuery = createSelectQuery("test", NULL);
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);

    int count = 0;
    while (next(scanner)) {
        count++;
    }
    assert(count == 4);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);


    struct Predicate* predicate = createPredicate();
    addCondition(predicate, "name", constant("abuabu"), EQUAL);
    struct DeleteQuery* deleteQuery = createDeleteQuery("test", predicate);
    performDeleteQuery(database, deleteQuery);
    destroyDeleteQuery(deleteQuery);

    selectQuery = createSelectQuery("test", NULL);
    scanner = performSelectQuery(database, selectQuery);

    count = 0;
    while (next(scanner)) {
        assert(strcmp(getString(scanner, "name").value, "abuabu") != 0);
        count++;
    }
    assert(count == 2);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);
    
    printf("Test successfully finished\n\n");
}

void testDeleteAllData() {
    printf("Starting delete all test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");

    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(12.5f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(-5));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("asdf"));
    addInsertionField(query, "floating", constant(1246.2f));
    addInsertionField(query, "number", constant(182341));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("qwer"));
    addInsertionField(query, "floating", constant(-47.214f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    destroyInsertQuery(query);

    struct SelectQuery* selectQuery = createSelectQuery("test", NULL);
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);

    int count = 0;
    while (next(scanner)) {
        count++;
    }
    assert(count == 4);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    struct DeleteQuery* deleteQuery = createDeleteQuery("test", NULL);
    performDeleteQuery(database, deleteQuery);
    destroyDeleteQuery(deleteQuery);

    selectQuery = createSelectQuery("test", NULL);
    scanner = performSelectQuery(database, selectQuery);

    assert(next(scanner) == false);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);
    
    printf("Test successfully finished\n\n");
}

void testUpdateData() {
    printf("Starting update test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");

    addInsertionField(query, "name", constant("abuabu"));
    addInsertionField(query, "floating", constant(12.5f));
    addInsertionField(query, "number", constant(123));
    performInsertQuery(database, query);

    destroyInsertQuery(query);

    struct Predicate* predicate = createPredicate();
    addCondition(predicate, "floating", constant(12.5f), EQUAL);
    struct UpdateQuery* updateQuery = createUpdateQuery("test", "name", constant("Steeve"), predicate);
    performUpdateQuery(database, updateQuery);
    destroyUpdateQuery(updateQuery);

    struct SelectQuery* selectQuery = createSelectQuery("test", NULL);
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);

    assert(next(scanner));
    assert(strcmp(getString(scanner, "name").value, "Steeve") == 0);
    assert(next(scanner) == false);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);
    printf("Test successfully finished\n\n");
}

void testJoinSelect() {
    printf("Starting join test\n");
    struct Database* database = openDatabase("database");
    dropDatabase(database);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    createTable(database, schema);
    destroySchema(schema);

    schema = createSchema("fruit");
    addStringField(schema, "is_apple", 5);
    addIntField(schema, "price");
    createTable(database, schema);
    destroySchema(schema);

    struct InsertQuery* query = createInsertQuery("test");

    addInsertionField(query, "name", constant("Viktor"));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "name", constant("Steeve"));
    performInsertQuery(database, query);

    destroyInsertQuery(query);

    query = createInsertQuery("fruit");

    addInsertionField(query, "is_apple", constant("True"));
    addInsertionField(query, "price", constant(24));
    performInsertQuery(database, query);

    clearInsertQuery(query);
    addInsertionField(query, "is_apple", constant("False"));
    addInsertionField(query, "price", constant(11));

    destroyInsertQuery(query);


    struct Predicate* predicate = createPredicate();
    addCondition(predicate, "price", constant(20), GREATER);
    struct SelectQuery* selectQuery = createSelectQuery("test", predicate);
    joinTable(selectQuery, "fruit");
    struct ScanInterface* scanner = performSelectQuery(database, selectQuery);
    
    assert(next(scanner));
    assert(strcmp(getString(scanner, "name").value, "Viktor") == 0);
    assert(strcmp(getString(scanner, "is_apple").value, "True") == 0);
    assert(getInt(scanner, "price") == 24);
    assert(next(scanner));
    assert(strcmp(getString(scanner, "name").value, "Steeve") == 0);
    assert(strcmp(getString(scanner, "is_apple").value, "True") == 0);
    assert(getInt(scanner, "price") == 24);
    assert(next(scanner) == false);

    destroySelectQuery(selectQuery);
    destroyScanner(scanner);

    closeDatabase(database);

    printf("Test successfully finished\n");
}