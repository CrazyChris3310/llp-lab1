#include <stdint.h>
#include "data_type.h"

struct file_header {
    uint16_t table_of_tables;
    uint16_t free_table;
};

struct page_header {
    uint16_t table_id;
    uint16_t count;
    uint16_t upper;
    uint16_t lower;
    uint16_t nextPage;
};

struct item_id_data {
    uint16_t offset;
    uint16_t length;
};

struct column {
    enum data_type dataType;
    uint16_t length;
    void* value;
};

struct item_data {
    struct column* columns;
};