#include <stdlib.h>
#include <string.h>

#include "page_cacheing/cache_manager.h"
#include "user_interface/scanners/scanners.h"
#include "util/my_string.h"
#include "middleware/schema.h"

#define MAX_COL_NAME 60

#define TABLE_OF_TABLES_NAME "table_of_tables"
#define TABLE_OF_TABLES_TABLE_NAME_COLUMN "name"
#define TABLE_FIRST_PAGE_COLUMN_NAME "first_page"
#define TABLE_RECORD_SIZE_COLUMN_NAME "record_size"
#define TABLE_OF_TABLES_ID_COLUMN "id"

#define COLUMN_TABLE_NAME "table_of_columns"
#define COLUMN_TABLE_TABLE_ID_COLUMN "table_id"
#define COLUMN_TABLE_NAME_COLUMN "name"
#define COLUMN_TABLE_OFFSET_COLUMN "offset"
#define COLUMN_TABLE_LENGTH_COLUMN "length"
#define COLUMN_TABLE_TYPE_COLUMN "type"

struct TableManager {
    struct CacheManager* cacheManager;
    struct Schema* tableOfTables;
    struct Schema* columnTable;
};

void createTable(struct TableManager* tm, struct Schema* schema);

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

    // if (isNew) {
    //     createTable(tableManager, tableManager->tableOfTables);
    //     createTable(tableManager, tableManager->columnTable);
    // }
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

// Need ID counter
void createTable(struct TableManager* tm, struct Schema* schema) {
    struct PossibleOffset po = tm->cacheManager->fileManager->header.tableOfTables;
    struct TableScanner* tableScanner = createTableScanner(tm->cacheManager, tm->tableOfTables, !po.exists, po.offset);

    if (!po.exists) {
        tm->cacheManager->fileManager->header.tableOfTables.exists = true;
        tm->cacheManager->fileManager->header.tableOfTables.offset = getCurrentBlock(tableScanner);
    }

    int64_t tableId = getNextTableId(tm);

    insert(tableScanner);
    setVarchar(tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN, schema->name);
    setInt(tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME, -1);
    setInt(tableScanner, TABLE_RECORD_SIZE_COLUMN_NAME, schema->slotSize);
    setInt(tableScanner, TABLE_OF_TABLES_ID_COLUMN, tableId);

    destroyTableScanner(tableScanner);


    po = tm->cacheManager->fileManager->header.tableOfColumns;
    struct TableScanner* colScanner = createTableScanner(tm->cacheManager, tm->columnTable, !po.exists, po.offset);

    if (!po.exists) {
        tm->cacheManager->fileManager->header.tableOfColumns.exists = true;
        tm->cacheManager->fileManager->header.tableOfColumns.offset = getCurrentBlock(colScanner);
    }

    struct Field* field = schema->firstField;
    while (field != NULL) {
        insert(colScanner);
        setString(colScanner, COLUMN_TABLE_NAME_COLUMN, field->name);
        setInt(colScanner, COLUMN_TABLE_TABLE_ID_COLUMN, tableId);
        setInt(colScanner, COLUMN_TABLE_TYPE_COLUMN, field->type);
        setInt(colScanner, COLUMN_TABLE_LENGTH_COLUMN, field->len);
        field = field->next;
    }
    destroyTableScanner(colScanner);
}

// handle problem that page with strign may be freed by bufferManager
struct Schema* findTableSchema(struct TableManager* tm, char* tableName) {
    struct TableScanner* tableScanner = createTableScanner(tm->cacheManager, tm->tableOfTables, false, tm->cacheManager->fileManager->header.tableOfTables.offset);
    struct String tblName = (struct String){ .value = tableName, .lenght = strlen(tableName) };
    int64_t firstPageOffset;
    size_t recordSize;
    size_t tableId;
    while (next(tableScanner)) {
        if (equals(getString(tableScanner, TABLE_OF_TABLES_TABLE_NAME_COLUMN), tblName)) {
            firstPageOffset = getInt(tableScanner, TABLE_FIRST_PAGE_COLUMN_NAME);
            recordSize = getInt(tableScanner, TABLE_RECORD_SIZE_COLUMN_NAME);
            tableId = getInt(tableScanner, TABLE_OF_TABLES_ID_COLUMN);
            break;
        }
    }

    destroyTableScanner(tableScanner);

    struct Schema* schema = createSchema(tableName);
    struct TableScanner* colScanner = createTableScanner(tm->cacheManager, tm->columnTable, false, tm->cacheManager->fileManager->header.tableOfColumns.offset);
    while(next(colScanner)) {
        if (getInt(colScanner, COLUMN_TABLE_TABLE_ID_COLUMN) == tableId) {
            struct String name = getString(colScanner, COLUMN_TABLE_NAME_COLUMN);
            int64_t fieldType = getInt(colScanner, COLUMN_TABLE_TYPE_COLUMN);
            int64_t length = getInt(colScanner, COLUMN_TABLE_LENGTH_COLUMN);
            
            addField(schema, name.value, fieldType, length);
        }
    }
    schema->slotSize = recordSize;
    schema->startBlock = firstPageOffset;
    destroyTableScanner(colScanner);
    return schema;
}