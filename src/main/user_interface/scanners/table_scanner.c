#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "util/my_string.h"
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "middleware/page_record.h"
#include "scanner_declarations.h"

static bool __tableScanNextFunction(void* ptr);
static void __insertRecordIntoTableScanner(void* ptr);
void __setIntegerToTableScanner(void* ptr, char* field, int64_t value);
void __setFloatToTableScanner(void* ptr, char* field, float value);
void __setBoolToTableScanner(void* ptr, char* field, bool value);
void __setStringToTableScanner(void* ptr, char* field, struct String value);
void __setVarcharToTableScanner(void* ptr, char* field, char* value);

int64_t __getIntegerFromTableScanner(void* ptr, char* field);
float __getFloatFromTableScanner(void* ptr, char* field);
bool __getBoolFromTableScanner(void* ptr, char* field);
struct String __getStringFromTableScanner(void* ptr, char* field);

static void moveScannerToBlock(struct TableScanner* scanner, size_t blockId);
static size_t moveScannerToNewBlock(struct TableScanner* scanner, bool hasCurrent);

/*
    search for the first page of given table in file and set current scanner before first record of this table
*/
struct TableScanner* createTableScanner(struct CacheManager* cm, struct Schema* schema, bool isNew, size_t blockStart) {
    struct TableScanner* scanner = malloc(sizeof(struct TableScanner));
    scanner->cacheManager = cm;
    scanner->schema = schema;
    scanner->blockId = blockStart;
    scanner->pageRecord = NULL;

    if (isNew) {
        moveScannerToNewBlock(scanner, false);
        // schema->startBlock = newBlockId;
    } else {
        moveScannerToBlock(scanner, blockStart);
    }

    scanner->goToNextRecord = __tableScanNextFunction;
    scanner->insertNextRecord = __insertRecordIntoTableScanner;

    scanner->getBool = __getBoolFromTableScanner;
    scanner->getFloat = __getFloatFromTableScanner;
    scanner->getInt = __getIntegerFromTableScanner;
    scanner->getString = __getStringFromTableScanner;

    scanner->setBool = __setBoolToTableScanner;
    scanner->setInt = __setIntegerToTableScanner;
    scanner->setFloat = __setFloatToTableScanner;
    scanner->setString = __setStringToTableScanner;
    scanner->setVarchar = __setVarcharToTableScanner;

    return scanner;
}

void destroyTableScanner(struct TableScanner* scanner) {
    destroyPageRecord(scanner->pageRecord);
    free(scanner);
}

size_t getCurrentBlock(struct TableScanner* scanner) {
    return scanner->blockId;
}

// getter to get values of current record

int64_t __getIntegerFromTableScanner(void* ptr, char* field) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    return getIntFromRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field});
}

float __getFloatFromTableScanner(void* ptr, char* field) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    return getFloatFromRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field});
}

bool __getBoolFromTableScanner(void* ptr, char* field) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    return getBoolFromRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field});
}

struct String __getStringFromTableScanner(void* ptr, char* field) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    return getStringFromRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field});
}

// setters only for update

void __setIntegerToTableScanner(void* ptr, char* field, int64_t value) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    setIntToRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field}, value);
}

void __setFloatToTableScanner(void* ptr, char* field, float value) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    setFloatToRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field}, value);
}

void __setBoolToTableScanner(void* ptr, char* field, bool value) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    setBoolToRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field}, value);
}

void __setStringToTableScanner(void* ptr, char* field, struct String value) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    setStringToRecord(scanner->pageRecord, (struct String){.lenght = strlen(field), .value = field}, value);
}

void __setVarcharToTableScanner(void* ptr, char* field, char* value) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    setStringToRecord(scanner->pageRecord, 
                    (struct String){.lenght = strlen(field), .value = field},
                    (struct String){.lenght = strlen(value), .value = value});
}

static void moveScannerToBlock(struct TableScanner* scanner, size_t blockId) {
    if (scanner->pageRecord != NULL) {
        destroyPageRecord(scanner->pageRecord);
    }
    scanner->pageRecord = createPageRecord(scanner->cacheManager, scanner->schema, blockId);
    scanner->blockId = blockId;
}

static void moveScannerToNextBlock(struct TableScanner* scanner) {
    if (scanner->pageRecord != NULL) {
        destroyPageRecord(scanner->pageRecord);
    }
    struct CachedPage* page = requestCachedPage(scanner->cacheManager, scanner->blockId);

    if (page->page->header->nextPage.exists) {
        scanner->pageRecord = createPageRecord(scanner->cacheManager, scanner->schema, page->page->header->nextPage.offset);
        scanner->blockId = page->page->header->nextPage.offset;
    } else {
        scanner->pageRecord = NULL;
        scanner->blockId = 0;
    }
}

static size_t moveScannerToNewBlock(struct TableScanner* scanner, bool hasCurrent) {
    struct PageHeader header;
    header.upper = sizeof(header);
    header.lower = scanner->cacheManager->fileManager->blockSize;
    header.count = 0;
    header.nextPage = (struct PossibleOffset){ .exists = false };

    size_t blockId = addNewBlock(scanner->cacheManager->fileManager, &header);

    if (hasCurrent) {
        struct CachedPage* cachedPage = requestCachedPage(scanner->cacheManager, scanner->blockId);
        cachedPage->page->header->nextPage = (struct PossibleOffset){ .exists = true, .offset = blockId };
        cachedPage->page->isDirty = true;
        releaseCachedPage(scanner->cacheManager, cachedPage);
    }

    scanner->pageRecord = createPageRecord(scanner->cacheManager, scanner->schema, blockId);
    clearAllRecords(scanner->pageRecord);
    scanner->blockId = blockId;
    return blockId;
}

static bool isLastBlock(struct TableScanner* scanner) {
    struct CachedPage* page = requestCachedPage(scanner->cacheManager, scanner->blockId);
    bool isLast = !page->page->header->nextPage.exists;
    releaseCachedPage(scanner->cacheManager, page);
    return isLast;
}

static bool __tableScanNextFunction(void* ptr) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    bool found = goToNextRecord(scanner->pageRecord);
    while (!found) {
        if (isLastBlock(scanner)) {
            return false;
        }
        moveScannerToNextBlock(scanner);
        // moveScannerToBlock(scanner, scanner->blockId + 1);
        found = goToNextRecord(scanner->pageRecord);
    }
    return true;
}

static void __insertRecordIntoTableScanner(void* ptr) {
    struct TableScanner* scanner = (struct TableScanner*)ptr;
    bool found = insertNextRecord(scanner->pageRecord);
    while (!found) {
        if (isLastBlock(scanner)) {
            moveScannerToNewBlock(scanner, true);
        } else {
            moveScannerToNextBlock(scanner);
            // moveScannerToBlock(scanner, scanner->blockId + 1);
        }
        found = insertNextRecord(scanner->pageRecord);
    }
}