#ifndef PAGE_H
#define PAGE_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/my_string.h"

struct Page;

struct Page allocatePage(size_t page_size);
void deallocate(struct Page* page);

int64_t getInt(struct Page* page, size_t offset);
float getFloat(struct Page* page, size_t offset);
bool getBool(struct Page* page, size_t offset);
struct String getString(struct Page* page, size_t offset);

void setInt(struct Page* page, size_t offset, int64_t value);
void setFloat(struct Page* page, size_t offset, float value);
void setBool(struct Page* page, size_t offset, bool value);
void setString(struct Page* page, size_t offset, struct String value);

#endif