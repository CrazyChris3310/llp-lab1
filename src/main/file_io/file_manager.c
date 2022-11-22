#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "file_manager.h"

static int8_t readFileHeader(struct FileManager* fm) {
    if (fm->isNew) {
        fm->header.freePages = (struct PossibleOffset){ .exists=false };
        fm->header.tableOfColumns = (struct PossibleOffset){ .exists=false };
        fm->header.tableOfTables = (struct PossibleOffset){ .exists=false };
    } else {
        fseek(fm->file, 0, SEEK_SET);
        fread(&fm->header, sizeof(struct FileHeader), 1, fm->file);
    }
}

static int8_t writeFileHeader(struct FileManager* fm) {
    fseek(fm->file, 0, SEEK_SET);
    fwrite(&fm->header, sizeof(struct FileHeader), 1, fm->file);
}

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
    readFileHeader(manager);
    return manager;
}

// TODO: better to rename to destroy
void closeFileManager(struct FileManager* manager) {
    fclose(manager->file);
    free(manager);
}

// handle errors
int8_t readPage(struct FileManager* fm, size_t blockId, struct Page* page) {
    fseek(fm->file, sizeof(struct FileHeader) + blockId * fm->blockSize, SEEK_SET);
    size_t blocks_read = fread(page->data, fm->blockSize, 1, fm->file);
    page->header = (struct PageHeader*)page->data;
    page->isDirty = false;
    return blocks_read == 1 ? 0 : 1;
}

int8_t writePage(struct FileManager* fm, size_t blockId, struct Page* page) {
    fseek(fm->file, sizeof(struct FileHeader) + blockId * fm->blockSize, SEEK_SET);
    size_t blocks_written = fwrite(page->data, fm->blockSize, 1, fm->file);
    return blocks_written == 1 ? 0 : 1;
}

// not sure if we should just use ftell or divide by blocks and go to the last position
size_t addNewBlock(struct FileManager* fm) {
    fseek(fm->file, 0, SEEK_END);
    size_t block_cnt = ftell(fm->file) / fm->blockSize;
    char data[fm->blockSize];
    fseek(fm->file, sizeof(struct FileHeader) + block_cnt * fm->blockSize, SEEK_SET);
    fwrite(data, fm->blockSize, 1, fm->file);

    return block_cnt;

    // char data[fm->blockSize];
    // fseek(fm->file, 0, SEEK_END);
    // fwrite(data, fm->blockSize, 1, data);
}

size_t getFileLength(struct FileManager* fm) {
    fseek(fm->file, 0, SEEK_END);
    return ftell(fm->file);
}