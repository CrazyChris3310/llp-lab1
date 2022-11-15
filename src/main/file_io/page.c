#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../util/my_string.h"

struct PossiblePage {
    bool exists;
    size_t offset;
};

struct PageHeader {
    size_t tableId;
    size_t count;
    size_t upper;
    size_t lower;
    struct PossiblePage nextPage;
};

struct Page {
    // struct PageHeader header;
    void* data;
};

// maybe better to allocate page in a heap
struct Page* allocatePage(size_t page_size) {
    struct Page* page = malloc(sizeof(struct Page));
    void* data = malloc(page_size - sizeof(struct PageHeader));
    page->data = data;
    return page;
}

void deallocatePage(struct Page* page) {
    free(page->data);
    free(page);
}

// all offses considering header
int64_t getInt(struct Page* page, size_t offset) {
    return *(int64_t*)(page->data + offset);
}

float getFloat(struct Page* page, size_t offset) {
    return *(float*)(page->data + offset);
}

bool getBool(struct Page* page, size_t offset) {
    return *(bool*)(page->data + offset);
}

struct String getString(struct Page* page, size_t offset) {
    struct String str = {};
    str.lenght = getInt(page, offset);
    str.value = (char*)(page->data + offset + sizeof(int64_t));
    return str;
}

void setInt(struct Page* page, size_t offset, int64_t value) {
    *(int64_t*)(page->data + offset) = value;
}

void setFloat(struct Page* page, size_t offset, float value) {
    *(float*)(page->data + offset) = value;
}

void setBool(struct Page* page, size_t offset, bool value) {
    *(bool*)(page->data + offset) = value;
}

// maybe replaced with copyString() function
void setString(struct Page* page, size_t offset, struct String value) {
    setInt(page, offset, value.lenght);
    char* str_dest = (char*)(page->data + offset + sizeof(int64_t));
    for (size_t i = 0; i < value.lenght; ++i) {
        str_dest[i] = value.value[i];
    }
}