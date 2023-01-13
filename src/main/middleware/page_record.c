
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "util/my_string.h"
#include "page_cacheing/cache_manager.h"
#include "schema.h"

#define USED 1
#define EMPTY 0

struct PageRecord {
    size_t blockId;
    struct CacheManager* cacheManager;
    struct Schema* schema;
    size_t id;
};

static size_t calculateRecordFieldOffset(struct Page* page, struct Schema* schema, size_t recordId, struct String field);

struct PageRecord* createPageRecord(struct CacheManager* cm, struct Schema* schema, size_t blockId) {
    struct PageRecord* record = malloc(sizeof(struct PageRecord));
    record->blockId = blockId;
    record->schema = schema;
    record->cacheManager = cm;
    // WTF???
    record->id = -1;
    return record;
}

void destroyPageRecord(struct PageRecord* record) {
    free(record);
}

int64_t getIntFromRecord(struct PageRecord* record, struct String field) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    int64_t result = getPageInt(cachedPage->page, offset);
    releaseCachedPage(record->cacheManager, cachedPage);
    return result;
}

struct String getStringFromRecord(struct PageRecord* record, struct String field) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    struct String result = getPageString(cachedPage->page, offset);
    releaseCachedPage(record->cacheManager, cachedPage);
    return result;
}

bool getBoolFromRecord(struct PageRecord* record, struct String field) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    bool result = getPageBool(cachedPage->page, offset);
    releaseCachedPage(record->cacheManager, cachedPage);
    return result;
}

float getFloatFromRecord(struct PageRecord* record, struct String field) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    float result = getPageFloat(cachedPage->page, offset);
    releaseCachedPage(record->cacheManager, cachedPage);
    return result;
}

void* getFieldFromRecord(struct PageRecord* record, struct String field) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    void* result = getRawData(cachedPage->page, offset);
    releaseCachedPage(record->cacheManager, cachedPage);
    return result;
}

static size_t calculateRecordFieldOffset(struct Page* page, struct Schema* schema, size_t recordId, struct String field) {
    struct PossibleOffset po = getFieldOffset(schema, field);
    if (po.exists) {
        return po.offset + schema->slotSize * recordId;
    }
    // FIXME:
    return -1;
}

// static size_t getNextFieldOffset(struct Page* page, struct Field* field, size_t offset) {
//     if (field->type != STRING) {
//         return offset + field->len;
//     } else {
//         size_t strLen = getPageSizeT(page, offset);
//         return offset + sizeof(size_t) + strLen;
//     }
// }

// // FIXME: field may be not found
// static size_t calculateRecordFieldOffset(struct Page* page, struct Schema* schema, size_t recordId, struct String field) {
//     size_t recordOffset = getPageSizeT(page, recordId * sizeof(size_t));
//     void* data = getRawData(page, recordOffset);

//     struct Field* current = schema->firstField;
//     while (current != NULL) {
//         if (compareStrings(current->name, field) == 0) {
//             break;
//         }
//         recordOffset = getNextFieldOffset(page, current, recordOffset);
//     }
//     return recordOffset;
// }

// WARNING: this is only for a fixed size fields

void setIntToRecord(struct PageRecord* record, struct String field, int64_t value) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    setPageInt(cachedPage->page, offset, value);
    releaseCachedPage(record->cacheManager, cachedPage);
}

void setFloatToRecord(struct PageRecord* record, struct String field, float value) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    setPageFloat(cachedPage->page, offset, value);
    releaseCachedPage(record->cacheManager, cachedPage);
}

void setBoolToRecord(struct PageRecord* record, struct String field, bool value) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    setPageBool(cachedPage->page, offset, value);
    releaseCachedPage(record->cacheManager, cachedPage);
}

void setStringToRecord(struct PageRecord* record, struct String field, struct String value) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    size_t offset = calculateRecordFieldOffset(cachedPage->page, record->schema, record->id, field);
    setPageString(cachedPage->page, offset, value);
    releaseCachedPage(record->cacheManager, cachedPage);
}

static bool recordIsValid(struct PageRecord* record) {
    return (record->id + 1) * record->schema->slotSize <= record->cacheManager->fileManager->blockSize - sizeof(struct PageHeader);
}

static void setTakenFlag(struct PageRecord *record, bool value) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    setPageBool(cachedPage->page, record->id * record->schema->slotSize, value);
    releaseCachedPage(record->cacheManager, cachedPage);
}

void deletePageRecord(struct PageRecord *record) {
    setTakenFlag(record, EMPTY);
}

static bool searchAfter(struct PageRecord* record, bool flag) {
    while (recordIsValid(record)) {
        record->id += 1;
        struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
        bool res = getPageBool(cachedPage->page, record->id * record->schema->slotSize);
        releaseCachedPage(record->cacheManager, cachedPage); 
        if (res == flag) {
            return true;
        }
        // record->id += 1;
    }
    return false;
}

bool goToNextRecord(struct PageRecord* record) {
    return searchAfter(record, USED);
}

bool insertNextRecord(struct PageRecord* record) {
    bool found = searchAfter(record, EMPTY);
    if (found) {
        setTakenFlag(record, USED);
    }
    return found;
}

void clearAllRecords(struct PageRecord* record) {
    struct CachedPage* cachedPage = requestCachedPage(record->cacheManager, record->blockId);
    while (recordIsValid(record)) {
        record->id += 1;
        setPageBool(cachedPage->page, record->id * record->schema->slotSize, EMPTY);
        // record->id += 1;
    }
    releaseCachedPage(record->cacheManager, cachedPage); 
    record->id = -1;
}