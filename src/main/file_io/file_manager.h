#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdbool.h>
#include "page.h"

struct FileManager {
    FILE* file;
    size_t blockSize;
    bool isNew;
};

// handle errors
struct FileManager* createFileManager(char* filename, size_t blockSize);

void closeFileManager(struct FileManager* manager);

// handle errors
int readPage(struct FileManager* fm, size_t blockId, struct Page* page);

int writePage(struct FileManager* fm, size_t blockId, struct Page* page);

// not sure if we should just use ftell or divide by blocks and go to the last position
size_t addNewBlock(struct FileManager* fm);

#endif