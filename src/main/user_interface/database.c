#include <stdlib.h>
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"

// struct Database {
//     struct FileManager* fileManager;
//     struct CacheManager* cacheManager;
// };

// // handle case with block_size when database already exists 
// struct Database* openDatabase(char* filename, size_t blockSize, size_t poolSize) {
//     struct Database* db = malloc(sizeof(struct Database));
//     db->fileManager = createFileManager(filename, blockSize);
//     db->cacheManager = createCacheManager(db->fileManager, poolSize);
// }

// void closeDatabase(struct Database* db) {
//     destroyCacheManager(db->cacheManager);
//     closeFileManager(db->fileManager);
//     free(db);
// }