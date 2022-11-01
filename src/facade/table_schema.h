#ifndef TABLE_SCHEMA_H
#define TABLE_SCHEMA_H

#include "data_type.h"
#include <stdint.h>

struct column_data {
    char* name;
    enum data_type type;
};

struct table_schema {
    size_t id;
    char* name;
    size_t size; // amount of columns
    size_t next_page;
    struct column_data* columns;
};

#endif