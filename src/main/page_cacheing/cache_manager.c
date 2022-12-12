#include "cache_manager.h"
#include <stdlib.h>

struct CacheManager* createCacheManager(struct FileManager* fileManager, size_t poolSize) {
    struct CacheManager* cacheManager = malloc(sizeof(struct CacheManager));
    cacheManager->pagePool = malloc(sizeof(struct CachedPage) * poolSize);
    for (size_t i = 0; i < poolSize; i++) {
        cacheManager->pagePool[i].page = allocatePage(fileManager->blockSize);
        cacheManager->pagePool[i].usesCount = 0;
        cacheManager->pagePool[i].isNew = true;
    }
    cacheManager->poolSize = poolSize;
    cacheManager->fileManager = fileManager;
    return cacheManager;
}

// FIXME: maybe should free also fileManager
void destroyCacheManager(struct CacheManager* cacheManager) {
    free(cacheManager->pagePool);
    free(cacheManager);
}

void flushAllPages(struct CacheManager* cacheManager) {
    for (size_t i = 0; i < cacheManager->poolSize; ++i) {
        struct CachedPage buf = cacheManager->pagePool[i];
        writePage(cacheManager->fileManager, buf.blockId, buf.page);
    }
}

static struct CachedPage* findNotUsedCachedPage(struct CacheManager* cm) {
    for (size_t i = 0; i < cm->poolSize; ++i) {
        if (cm->pagePool[i].usesCount <= 0) {
            return &(cm->pagePool[i]);
        }
    }
    return NULL;
}

static struct CachedPage* findLoadedCachedPage(struct CacheManager* cm, size_t blockId) {
    for (size_t i = 0; i < cm->poolSize; ++i) {
        if (cm->pagePool[i].blockId == blockId) {
            return &(cm->pagePool[i]);
        }
    }
    return NULL;
}

struct CachedPage* requestCachedPage(struct CacheManager* cm, size_t blockId) {
    struct CachedPage* buf = findLoadedCachedPage(cm, blockId);
    if (buf != NULL) {
        buf->usesCount += 1;
        return buf;
    }
    buf = findNotUsedCachedPage(cm);
    if (buf == NULL) {
        // better to return error here
        return NULL;
    }
    if (buf->page->isDirty) {
        writePage(cm->fileManager, buf->blockId, buf->page);
    }
    readPage(cm->fileManager, blockId, buf->page);
    buf->usesCount += 1;
    buf->blockId = blockId;
    return buf;
}

void releaseCachedPage(struct CacheManager* cm, struct CachedPage* cachedPage) {
    cachedPage->usesCount -= 1;
}