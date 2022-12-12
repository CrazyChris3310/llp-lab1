#ifndef SCANNERS_DECLARATIONS_H
#define SCANNERS_DECLARATIONS_H

#include <inttypes.h>

struct TableScanner {
    struct Schema* schema;
    struct CacheManager* cacheManager;
    struct PageRecord* pageRecord;
    size_t blockId;

    bool(*goToNextRecord)(void* ptr);
    void(*insertNextRecord)(void* ptr);

    int64_t(*getInt)(void* ptr, char* field);
    struct String(*getString)(void* ptr, char* filed);
    float(*getFloat)(void* ptr, char* field);
    bool(*getBool)(void* ptr, char* field);

    void(*setInt)(void* ptr, char* field, int64_t value);
    void(*setString)(void* ptr, char* field, struct String value);
    void(*setBool)(void* ptr, char* field, bool value);
    void(*setFloat)(void* ptr, char* field, float value);
    void(*setVarchar)(void* ptr, char* field, char* value);
};


#endif 