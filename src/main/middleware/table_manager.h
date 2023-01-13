#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "middleware/schema.h"
#include "page_cacheing/cache_manager.h"

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

struct TableManager* createTableManager(struct CacheManager* cacheManager);
void destroyTableManager(struct TableManager* tm);

void createDatabaseTable(struct TableManager* tm, struct Schema* schema);
struct Schema* findTableSchema(struct TableManager* tm, char* tableName);

#endif