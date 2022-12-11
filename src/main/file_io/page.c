#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

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
    return *(int64_t*)(page->data + getRealOffset(offset));
}

size_t getPageSizeT(struct Page* page, size_t offset) {
    return *(size_t*)(page->data + getRealOffset(offset));
}

float getPageFloat(struct Page* page, size_t offset) {
    return *(float*)(page->data + getRealOffset(offset));
}

bool getPageBool(struct Page* page, size_t offset) {
    return *(bool*)(page->data + getRealOffset(offset));
}

struct String getPageString(struct Page* page, size_t offset) {
    struct String str = {};
    str.lenght = getPageSizeT(page, offset);
    str.value = (char*)(page->data + getRealOffset(offset) + sizeof(size_t));
    return str;
}

void setPageInt(struct Page* page, size_t offset, int64_t value) {
    page->isDirty = true;
    *(int64_t*)(page->data + getRealOffset(offset)) = value;
}

void setPageSizeT(struct Page* page, size_t offset, size_t value) {
    page->isDirty = true;
    *(size_t*)(page->data + getRealOffset(offset)) = value;
}

void setPageFloat(struct Page* page, size_t offset, float value) {
    page->isDirty = true;
    *(float*)(page->data + getRealOffset(offset)) = value;
}

void setPageBool(struct Page* page, size_t offset, bool value) {
    page->isDirty = true;
    *(bool*)(page->data + getRealOffset(offset)) = value;
}

// maybe replaced with copyString() function
void setPageString(struct Page* page, size_t offset, struct String value) {
    page->isDirty = true;
    setPageSizeT(page, offset, value.lenght);
    char* str_dest = (char*)(page->data + getRealOffset(offset) + sizeof(size_t));
    for (size_t i = 0; i < value.lenght; ++i) {
        str_dest[i] = value.value[i];
    }
}

void* getRawData(struct Page* page, size_t offset) {
    return page->data + getRealOffset(offset);
}