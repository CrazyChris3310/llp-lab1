#include "cache_manager.h"
#include <stdlib.h>

struct CacheManager* createCacheManager(struct FileManager* fileManager, size_t poolSize) {
    struct CacheManager* cacheManager = malloc(sizeof(struct CacheManager));
    cacheManager->pagePool = malloc(sizeof(struct CachedPage) * poolSize);
    for (size_t i = 0; i < poolSize; i++) {
        cacheManager->pagePool[i].page = allocatePage(fileManager->blockSize);
        cacheManager->pagePool[i].usesCount = 0;
        cacheManager->pagePool[i].blockId = (struct PossibleValue) { .exists = false, .value = 0};
    }
    cacheManager->poolSize = poolSize;
    cacheManager->fileManager = fileManager;
    return cacheManager;
}

// FIXME: maybe should free also fileManager
void destroyCacheManager(struct CacheManager* cacheManager) {
    flushAllPages(cacheManager);
    for (size_t i = 0; i < cacheManager->poolSize; ++i) {
        deallocatePage(cacheManager->pagePool[i].page);
    }
    free(cacheManager->pagePool);
    free(cacheManager);
}

void clearCachedPages(struct CacheManager* cacheManager) {
    for (size_t i = 0; i < cacheManager->poolSize; ++i) {
        cacheManager->pagePool[i].usesCount = 0;
        cacheManager->pagePool[i].blockId = (struct PossibleValue) { .exists = false, .value = 0};
    }
}

void flushAllPages(struct CacheManager* cacheManager) {
    for (size_t i = 0; i < cacheManager->poolSize; ++i) {
        struct CachedPage buf = cacheManager->pagePool[i];
        if (buf.page->isDirty) {
            writePage(cacheManager->fileManager, buf.blockId.value, buf.page);
        }
    }
}

static struct CachedPage* findNotUsedCachedPage(struct CacheManager* cm) {
    for (size_t i = 0; i < cm->poolSize; ++i) {
        if (!cm->pagePool[i].blockId.exists) {
            return &(cm->pagePool[i]);
        }
    }

    for (size_t i = 0; i < cm->poolSize; ++i) {
        if (cm->pagePool[i].usesCount <= 0) {
            return &(cm->pagePool[i]);
        }
    }
    return NULL;
}

static struct CachedPage* findLoadedCachedPage(struct CacheManager* cm, size_t blockId) {
    for (size_t i = 0; i < cm->poolSize; ++i) {
        if (cm->pagePool[i].blockId.exists && cm->pagePool[i].blockId.value == blockId) {
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
        return NULL;
    }
    if (buf->page->isDirty) {
        writePage(cm->fileManager, buf->blockId.value, buf->page);
    }
    readPage(cm->fileManager, blockId, buf->page);
    buf->usesCount += 1;
    buf->blockId = (struct PossibleValue){.exists = true, .value = blockId};
    return buf;
}

void releaseCachedPage(struct CacheManager* cm, struct CachedPage* cachedPage) {
    cachedPage->usesCount -= 1;
}