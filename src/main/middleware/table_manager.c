#include <stdlib.h>
#include <string.h>

#include "page_cacheing/cache_manager.h"
#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "util/my_string.h"
#include "middleware/schema.h"
#include "middleware/table_manager.h"
#include "util/linked_list.h"

#define MAX_COL_NAME 60

void createDatabaseTable(struct TableManager* tm, struct Schema* schema);

struct TableManager* createTableManager(struct CacheManager* cacheManager) {
    struct TableManager* tableManager = malloc(sizeof(struct TableManager));
    tableManager->cacheManager = cacheManager;

    struct Schema* schema = createSchema(TABLE_OF_TABLES_NAME);
    addStringField(schema, TABLE_OF_TABLES_TABLE_NAME_COLUMN, MAX_COL_NAME);
    addIntField(schema, TABLE_FIRST_PAGE_COLUMN_NAME);
    addIntField(schema, TABLE_RECORD_SIZE_COLUMN_NAME);
    addIntField(schema, TABLE_OF_TABLES_ID_COLUMN);
    tableManager->tableOfTables = schema;

    schema = createSchema(COLUMN_TABLE_NAME);
    addIntField(schema, COLUMN_TABLE_TABLE_ID_COLUMN);
    addIntField(schema, COLUMN_TABLE_LENGTH_COLUMN);
    addIntField(schema, COLUMN_TABLE_TYPE_COLUMN);
    addStringField(schema, COLUMN_TABLE_NAME_COLUMN, MAX_COL_NAME);
    tableManager->columnTable = schema;
    
    return tableManager;
}

void destroyTableManager(struct TableManager* tm) {
    destroySchema(tm->tableOfTables);
    destroySchema(tm->columnTable);
    free(tm);
}

static int64_t getNextTableId(struct TableManager* tm) {
    return tm->cacheManager->fileManager->header.next_table_id++;
}

void createDatabaseTable(struct TableManager* tm, struct Schema* schema) {
    struct PossibleValue po = tm->cacheManager->fileManager->header.tableOfTables;
    struct TableScanner* tableScanner = createTableScanner(tm->cacheManager, tm->tableOfTables, !po.exists, po.value);

    if (!po.exists) {
        tm->cacheManager->fileManager->header.tableOfTables.exists = true;
        tm->cacheManager->fileManager->header.tableOfTables.value = getCurrentBlock(tableScanner);
    } 

    int64_t tableId = getNextTableId(tm);

    insert((struct ScanInterface*)tableScanner);
    setVarchar((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN, schema->name);
    setInt((struct ScanInterface*)tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME, -1);
    setInt((struct ScanInterface*)tableScanner, TABLE_RECORD_SIZE_COLUMN_NAME, schema->slotSize);
    setInt((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_ID_COLUMN, tableId);

    destroyScanner((struct ScanInterface*)tableScanner);


    po = tm->cacheManager->fileManager->header.tableOfColumns;
    struct TableScanner* colScanner = createTableScanner(tm->cacheManager, tm->columnTable, !po.exists, po.value);

    if (!po.exists) {
        tm->cacheManager->fileManager->header.tableOfColumns.exists = true;
        tm->cacheManager->fileManager->header.tableOfColumns.value = getCurrentBlock(colScanner);
    }

    struct LinkedList* fields = schema->fields;
    struct ListIterator* iter = createListIterator(fields);
    while (iteratorHasNext(iter)) {
        struct Field* field = (struct Field*)iteratorNext(iter);
        insert((struct ScanInterface*)colScanner);
        setString((struct ScanInterface*)colScanner, COLUMN_TABLE_NAME_COLUMN, field->name);
        setInt((struct ScanInterface*)colScanner, COLUMN_TABLE_TABLE_ID_COLUMN, tableId);
        setInt((struct ScanInterface*)colScanner, COLUMN_TABLE_TYPE_COLUMN, field->type);
        setInt((struct ScanInterface*)colScanner, COLUMN_TABLE_LENGTH_COLUMN, field->len);
    }
    freeListIterator(iter);
    destroyScanner((struct ScanInterface*)colScanner);

    writeFileHeader(tm->cacheManager->fileManager);
}

struct Schema* findTableSchema(struct TableManager* tm, char* tableName) {
    struct TableScanner* tableScanner = createTableScanner(tm->cacheManager, tm->tableOfTables, false, tm->cacheManager->fileManager->header.tableOfTables.value);
    struct String tblName = (struct String){ .value = tableName, .lenght = strlen(tableName) };
    int64_t firstPageOffset;
    size_t recordSize;
    size_t tableId;

    bool found = false;
    while (next((struct ScanInterface*)tableScanner)) {
        if (equals(getString((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN), tblName)) {
            firstPageOffset = getInt((struct ScanInterface*)tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME);
            recordSize = getInt((struct ScanInterface*)tableScanner, TABLE_RECORD_SIZE_COLUMN_NAME);
            tableId = getInt((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_ID_COLUMN);
            found = true;
            break;
        }
    }

    if (!found) {
        return NULL;
    }

    destroyScanner((struct ScanInterface*)tableScanner);

    struct Schema* schema = createSchema(tableName);
    schema->shouldClear = true;
    struct TableScanner* colScanner = createTableScanner(tm->cacheManager, tm->columnTable, false, tm->cacheManager->fileManager->header.tableOfColumns.value);
    while(next((struct ScanInterface*)colScanner)) {
        if (getInt((struct ScanInterface*)colScanner, COLUMN_TABLE_TABLE_ID_COLUMN) == tableId) {
            struct String name = getString((struct ScanInterface*)colScanner, COLUMN_TABLE_NAME_COLUMN);
            int64_t fieldType = getInt((struct ScanInterface*)colScanner, COLUMN_TABLE_TYPE_COLUMN);
            int64_t length = getInt((struct ScanInterface*)colScanner, COLUMN_TABLE_LENGTH_COLUMN);
            
            addField(schema, name.value, fieldType, length);
        }
    }
    schema->slotSize = recordSize;
    schema->startBlock = firstPageOffset;
    destroyScanner((struct ScanInterface*)colScanner);
    return schema;
}

void dropDatabaseTable(struct TableManager* tm, char* tableName) {
    struct PossibleValue po = tm->cacheManager->fileManager->header.tableOfTables;
    struct TableScanner* tableScanner = createTableScanner(tm->cacheManager, tm->tableOfTables, !po.exists, po.value);
    int64_t tableId = -1;
    int64_t firstPage = -1;
    while (next((struct ScanInterface*)tableScanner)) {
        if (strcmp(getString((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN).value, tableName) == 0) {
            tableId = getInt((struct ScanInterface*)tableScanner, TABLE_OF_TABLES_ID_COLUMN);
            firstPage = getInt((struct ScanInterface*)tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME);
            deleteRecord((struct ScanInterface*)tableScanner);
        }
    }
    destroyScanner((struct ScanInterface*)tableScanner);

    po = tm->cacheManager->fileManager->header.tableOfColumns;
    struct TableScanner* colScanner = createTableScanner(tm->cacheManager, tm->columnTable, !po.exists, po.value);
    while(next((struct ScanInterface*)colScanner)) {
        if (getInt((struct ScanInterface*)colScanner, COLUMN_TABLE_TABLE_ID_COLUMN) == tableId) {
            deleteRecord((struct ScanInterface*)colScanner);
        }
    }
    destroyScanner((struct ScanInterface*)colScanner);

    if (firstPage != -1) {
        appendPagesToFreeList(tm->cacheManager, firstPage);
    }
}

