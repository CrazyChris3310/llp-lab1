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