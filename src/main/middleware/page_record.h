
#include <inttypes.h>
#include <stdbool.h>
#include "util/my_string.h"
#include "page_cacheing/cache_manager.h"
#include "schema.h"

#define USED 1
#define EMPTY 0

struct PageRecord {
    size_t blockId;
    struct CacheManager* cacheManager;
    struct Schema* schema;
};

struct PageRecord* createPageRecord(struct CacheManager* cm, struct Schema* schema, size_t blockId);
void destroyPageRecord(struct PageRecord* record);

int64_t getIntFromRecord(struct PageRecord* record, struct String field);
struct String getStringFromRecord(struct PageRecord* record, struct String field);
bool getBoolFromRecord(struct PageRecord* record, struct String field);
float getFloatFromRecord(struct PageRecord* record, struct String field) ;

void setIntToRecord(struct PageRecord* record, struct String field, int64_t value);
void setFloatToRecord(struct PageRecord* record, struct String field, float value);
void setBoolToRecord(struct PageRecord* record, struct String field, bool value);
void setStringToRecord(struct PageRecord* record, struct String field, struct String value);

void deleteRecord(struct PageRecord *record);
bool goToNextRecord(struct PageRecord* record);
bool insertNextRecord(struct PageRecord* record);
void clearAllRecords(struct PageRecord* record);