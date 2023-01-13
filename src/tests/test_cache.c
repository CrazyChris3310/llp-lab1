#include <assert.h>
#include "page_cacheing/cache_manager.h"

char cmpFloat(float a, float b) {
    float res = a - b > 0 ? a - b : b - a;
    if (res - 0.000001 < 0)
        return 0;
    else if (a > b)
        return a;
    else
        return b;
}

void testDataNotLost() {
    struct FileManager* fm = createFileManager("database", 4096);
    struct CacheManager* cm = createCacheManager(fm, 1);

    struct CachedPage* page = requestCachedPage(cm, 1);
    setPageFloat(page->page, 37, 0.4);
    releaseCachedPage(cm, page);

    requestCachedPage(cm, 3);
    releaseCachedPage(cm, page);

    page = requestCachedPage(cm, 1);
    assert(cmpFloat(0.4, getPageFloat(page->page, 37)) == 0);
    printf("Value received: %f\n", getPageFloat(page->page, 37));
    releaseCachedPage(cm, page);

    destroyCacheManager(cm);
    destoryFileManager(fm);
}