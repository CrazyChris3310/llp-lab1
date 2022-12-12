#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "middleware/schema.h"
#include "page_cacheing/cache_manager.h"


struct TableManager* createTableManager(struct CacheManager* cacheManager);
void destroyTableManager(struct TableManager* tm);

void createTable(struct TableManager* tm, struct Schema* schema);
struct Schema* findTableSchema(struct TableManager* tm, char* tableName);

#endif