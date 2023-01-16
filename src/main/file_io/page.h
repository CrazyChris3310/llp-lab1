#ifndef PAGE_H
#define PAGE_H

#include <inttypes.h>
#include <stdbool.h>

struct PossibleValue {
    bool exists;
    size_t value;
};

struct PageHeader {
    struct PossibleValue nextPage;
};

struct Page {
    struct PageHeader* header;
    uint8_t* data;
    bool isDirty;
};

struct Page* allocatePage(size_t page_size);
void deallocatePage(struct Page* page);

int64_t getPageInt(struct Page* page, size_t offset);
size_t getPageSizeT(struct Page* page, size_t offset);
float getPageFloat(struct Page* page, size_t offset);
bool getPageBool(struct Page* page, size_t offset);
struct String getPageString(struct Page* page, size_t offset);

void setPageInt(struct Page* page, size_t offset, int64_t value);
void setPageSizeT(struct Page* page, size_t offset, size_t value);
void setPageFloat(struct Page* page, size_t offset, float value);
void setPageBool(struct Page* page, size_t offset, bool value);
void setPageString(struct Page* page, size_t offset, struct String value);

uint8_t* getRawData(struct Page* page, size_t offset);

#endif