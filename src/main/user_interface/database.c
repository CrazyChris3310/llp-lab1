#include <stdlib.h>
#include <stdbool.h>
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "user_interface/read_scan.h"
#include "middleware/table_manager.h"
#include "user_interface/query.h"
#include "user_interface/write_scan.h"

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
    closeFileManager(db->fileManager);
    free(db);
}

void createTable(struct Database* database, struct Schema* schema) {
    createDatabaseTable(database->tableManager, schema);
}

struct ScanInterface* performQuery(struct Database* database, char* tableName, struct SelectQuery* query) {
    struct Schema* schema = findTableSchema(database->tableManager, tableName);
    bool isNew = schema->startBlock == -1;

    struct ScanInterface* scanning = (struct ScanInterface*)createTableScanner(database->cacheManager, schema, isNew, schema->startBlock);
    struct ScanInterface* scan = scanning;
    if (query != NULL) {
        scan = (struct ScanInterface*)createSelectScanner(scanning, *query->predicate);
    }

    if (isNew && schema->startBlock != -1) {
        struct TableScanner* tableScanner = createTableScanner(database->cacheManager, database->tableManager->tableOfTables, false, database->fileManager->header.tableOfTables.offset);
        struct String tblName = (struct String){ .value = tableName, .lenght = strlen(tableName) };

        while (next((struct ScanInterface*)tableScanner)) {
            if (equals(getString((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN), tblName)) {
                setInt((struct ScanInterface*)tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME, schema->startBlock);
                break;
            }
        }

        destroy((struct ScanInterface*)tableScanner);
    }

    return scan;
}
