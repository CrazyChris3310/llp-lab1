#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include "file_io/file_manager.h"
#include "file_io/page.h"

struct CachedPage {
    struct Page* page;
    struct PossibleValue blockId;
    size_t usesCount;
    bool isNew;
};

struct CacheManager {
    struct CachedPage* pagePool;
    size_t poolSize;
    struct FileManager* fileManager;
};


struct CacheManager* createCacheManager(struct FileManager* fileManager, size_t poolSize);
void destroyCacheManager(struct CacheManager* CacheManager);
void flushAllPages(struct CacheManager* CacheManager);
struct CachedPage* requestCachedPage(struct CacheManager* cm, size_t blockId);
void releaseCachedPage(struct CacheManager* cm, struct CachedPage* CachedPage);

#endif 
