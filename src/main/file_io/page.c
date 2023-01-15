#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util/my_string.h"
#include "page.h"

// maybe better to allocate page in a heap
struct Page* allocatePage(size_t page_size) {
    struct Page* page = malloc(sizeof(struct Page));
    void* data = malloc(page_size);
    page->data = data;
    page->isDirty = false;
    page->header = data;
    return page;
}

void deallocatePage(struct Page* page) {
    free(page->data);
    free(page);
}

static size_t getRealOffset(size_t offset) {
    return sizeof(struct PageHeader) + offset;
}

int64_t getPageInt(struct Page* page, size_t offset) {
    int64_t result;
    memcpy(&result, page->data + getRealOffset(offset), sizeof(result));
    return result;
}

size_t getPageSizeT(struct Page* page, size_t offset) {
    size_t result;
    memcpy(&result, page->data + getRealOffset(offset), sizeof(result));
    return result;
}

float getPageFloat(struct Page* page, size_t offset) {
    float result;
    memcpy(&result, page->data + getRealOffset(offset), sizeof(result));
    return result;
}

bool getPageBool(struct Page* page, size_t offset) {
    bool result;
    memcpy(&result, page->data + getRealOffset(offset), sizeof(result));
    return result;
}

struct String getPageString(struct Page* page, size_t offset) {
    struct String str = {0};
    str.lenght = getPageSizeT(page, offset);
    str.value = (char*)(page->data + getRealOffset(offset) + sizeof(size_t));
    return str;
}

void setPageInt(struct Page* page, size_t offset, int64_t value) {
    page->isDirty = true;
    memcpy(page->data + getRealOffset(offset), &value, sizeof(value));
}

void setPageSizeT(struct Page* page, size_t offset, size_t value) {
    page->isDirty = true;
    memcpy(page->data + getRealOffset(offset), &value, sizeof(value));
}

void setPageFloat(struct Page* page, size_t offset, float value) {
    page->isDirty = true;
    memcpy(page->data + getRealOffset(offset), &value, sizeof(value));
}

void setPageBool(struct Page* page, size_t offset, bool value) {
    page->isDirty = true;
    memcpy(page->data + getRealOffset(offset), &value, sizeof(value));
}

void setPageString(struct Page* page, size_t offset, struct String value) {
    page->isDirty = true;
    setPageSizeT(page, offset, value.lenght);
    char* str_dest = (char*)(page->data + getRealOffset(offset) + sizeof(size_t));
    memcpy(str_dest, value.value, value.lenght);
    str_dest[value.lenght] = '\0';
}

uint8_t* getRawData(struct Page* page, size_t offset) {
    return page->data + getRealOffset(offset);
}