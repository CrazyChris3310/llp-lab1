#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "util/my_string.h"
#include "page_cacheing/cache_manager.h"
#include "file_io/file_manager.h"
#include "schema.h"
#include "user_interface/database.h"

void appendPagesToFreeList(struct CacheManager* cacheManager, int64_t blockId) {
    struct FileHeader* header = &cacheManager->fileManager->header;
    if (!header->freePages.exists) {
        header->freePages.exists = true;
        header->freePages.value = blockId;
        return;
    }
    int64_t res;
    struct PossibleValue next_block = header->freePages;
    while (next_block.exists) {
        res = next_block.value;
        struct CachedPage* cachedPage = requestCachedPage(cacheManager, res);
        next_block = cachedPage->page->header->nextPage;
        releaseCachedPage(cacheManager, cachedPage);
    }

    struct CachedPage* cachedPage = requestCachedPage(cacheManager, res);
    cachedPage->page->header->nextPage.exists = true;
    cachedPage->page->header->nextPage.value = blockId;
    releaseCachedPage(cacheManager, cachedPage);
}