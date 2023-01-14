#include <stdlib.h>
#include <stdbool.h>
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "user_interface/read_scan.h"
#include "middleware/table_manager.h"
#include "user_interface/query.h"
#include "user_interface/write_scan.h"
#include <assert.h>
#include "util/linked_list.h"

#define DEFAULT_BLOCK_SIZE 4096
#define DEFAULT_POOL_SIZE 10

struct Database {
    struct FileManager* fileManager;
    struct CacheManager* cacheManager;
    struct TableManager* tableManager;
};

// handle case with block_size when database already exists 
struct Database* openDatabase(char* filename) {
    struct Database* db = malloc(sizeof(struct Database));
    db->fileManager = createFileManager(filename, DEFAULT_BLOCK_SIZE);
    db->cacheManager = createCacheManager(db->fileManager, DEFAULT_POOL_SIZE);
    db->tableManager = createTableManager(db->cacheManager);

    return db;
}

void closeDatabase(struct Database* db) {
    destroyTableManager(db->tableManager);
    destroyCacheManager(db->cacheManager);
    destoryFileManager(db->fileManager);
    free(db);
}

void createTable(struct Database* database, struct Schema* schema) {
    createDatabaseTable(database->tableManager, schema);
}

static void setTableFirstBlock(struct Database* database, char* tableName, size_t firstBlock) {
    struct TableScanner* tableScanner = createTableScanner(database->cacheManager, database->tableManager->tableOfTables, false, database->fileManager->header.tableOfTables.offset);
        struct String tblName = (struct String){ .value = tableName, .lenght = strlen(tableName) };

        while (next((struct ScanInterface*)tableScanner)) {
            if (equals(getString((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN), tblName)) {
                setInt((struct ScanInterface*)tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME, firstBlock);
                break;
            }
        }

        destroyScanner((struct ScanInterface*)tableScanner);
}

struct ScanInterface* performSelectQuery(struct Database* database, struct SelectQuery* query) {
    struct Schema* schema = findTableSchema(database->tableManager, query->from);
    bool isNew = schema->startBlock == -1;

    struct ScanInterface* scanning = (struct ScanInterface*)createTableScanner(database->cacheManager, schema, isNew, schema->startBlock);
    struct ScanInterface* scan = scanning;
    if (query->predicate != NULL) {
        scan = (struct ScanInterface*)createSelectScanner(scanning, *query->predicate);
    }

    if (isNew && schema->startBlock != -1) {
        setTableFirstBlock(database, query->from, schema->startBlock);
    }

    return scan;
}

void performInsertQuery(struct Database* database, struct InsertQuery* query) {
    struct Schema* schema = findTableSchema(database->tableManager, query->into);
    assert(schema != NULL);
    bool isNew = schema->startBlock == -1;
    
    struct ScanInterface* scan = (struct ScanInterface*)createTableScanner(database->cacheManager, schema, isNew, schema->startBlock);

    if (isNew && schema->startBlock != -1) {
        setTableFirstBlock(database, query->into, schema->startBlock);
    }

    insert(scan);
    struct ListIterator* iterator = createListIterator(query->values);
    while (iteratorHasNext(iterator)) {
        struct Condition* condition = (struct Condition*)iteratorNext(iterator);
        setField(scan, condition->fieldName, condition->constant);
    }

    freeListIterator(iterator);
    destroyScanner(scan);
}

void performDeleteQuery(struct Database* database, struct DeleteQuery* query) {
    struct Schema* schema = findTableSchema(database->tableManager, query->from);
    bool isNew = schema->startBlock == -1;

    assert(!isNew);

    struct ScanInterface* scan = (struct ScanInterface*)createTableScanner(database->cacheManager, schema, isNew, schema->startBlock);
    scan = (struct ScanInterface*)createSelectScanner(scan, *query->predicate);

    while (next(scan)) {
        deleteRecord(scan);
    }

    destroyScanner(scan);
}

void performUpdateQuery(struct Database* database, struct UpdateQuery* query) {
    struct Schema* schema = findTableSchema(database->tableManager, query->table);
    bool isNew = schema->startBlock == -1;

    assert(!isNew);

    struct ScanInterface* scan = (struct ScanInterface*)createTableScanner(database->cacheManager, schema, isNew, schema->startBlock);
    scan = (struct ScanInterface*)createSelectScanner(scan, *query->predicate);

    while (next(scan)) {
        setField(scan, query->condition->fieldName, query->condition->constant);
    }

    destroyScanner(scan);
}

void dropDatabase(struct Database* database) {
    clearFile(database->fileManager);
}

struct Schema* findTable(struct Database* database, char* tableName) {
    return findTableSchema(database->tableManager, tableName);
}