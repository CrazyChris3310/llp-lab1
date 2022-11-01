#ifndef PAGE_H
#define PAGE_H

#include <stdint.h>
#include "data_type.h"
#include <stdbool.h>
#include <stdio.h>

struct file_header {
    size_t table_of_tables;
    size_t free_table;
    size_t table_of_columns;
};

struct maybe_page {
    bool exists;
    size_t offset;
};

struct page_header {
    size_t table_id;
    uint16_t count;
    uint16_t upper;
    uint16_t lower;
    struct maybe_page next_page;
};

struct item_id_data {
    uint16_t offset;
    uint16_t length; // amount of columns
};

struct column {
    enum data_type dataType;
    uint16_t length;
    void* value;
};

struct item_data {
    struct column* columns;
};

struct file_handle {
    FILE* file;
    struct file_header header;
};

#endif