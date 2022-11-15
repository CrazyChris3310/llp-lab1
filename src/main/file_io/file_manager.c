#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "page.h"

struct FileManager {
    FILE* file;
    size_t blockSize;
    bool isNew;
};

// handle errors
struct FileManager* createFileManager(char* filename, size_t blockSize) {
    struct FileManager* manager = malloc(sizeof(struct FileManager));
    manager->blockSize = blockSize;
    FILE* file = fopen(filename, "rb+");
    if (file == NULL) {
        file = fopen(filename, "wb+");
        manager->isNew = true;
    }
    manager->file = file;
    return manager;
}

void closeFileManager(struct FileManager* manager) {
    fclose(manager->file);
    free(manager);
}

// handle errors
int readPage(struct FileManager* fm, size_t blockId, struct Page* page) {
    fseek(fm->file, blockId * fm->blockSize, SEEK_SET);
    size_t blocks_read = fread(page->data, fm->blockSize, 1, fm->file);
    return blocks_read == 1 ? 0 : 1;
}

int writePage(struct FileManager* fm, size_t blockId, struct Page* page) {
    fseek(fm->file, blockId * fm->blockSize, SEEK_SET);
    size_t blocks_written = fwrite(page->data, fm->blockSize, 1, fm->file);
    return blocks_written == 1 ? 0 : 1;
}

// not sure if we should just use ftell or divide by blocks and go to the last position
size_t addNewBlock(struct FileManager* fm) {
    fseek(fm->file, 0, SEEK_END);
    size_t block_cnt = ftell(fm->file) / fm->blockSize;
    char data[fm->blockSize];
    fseek(fm->file, block_cnt * fm->blockSize, SEEK_SET);
    fwrite(data, fm->blockSize, 1, fm->file);

    return block_cnt;

    // char data[fm->blockSize];
    // fseek(fm->file, 0, SEEK_END);
    // fwrite(data, fm->blockSize, 1, data);
}