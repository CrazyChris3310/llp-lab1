#include "data_type.h"
#include "table_schema.h"
#include "file_operations.h"
#include <string.h>
#include "table_id_gen.h"

/* 
Create empty table
name - table name
*/
struct table_schema* prepare_table(char* name) {
    // allocate memory
    struct table_schema* table;
    table->name = name;
    table->size = 0;
}

/* 
Add new column with selected data type to a table
table - table that is being created to add column to
name - name of column
type - data type of column
*/
void add_column(struct table_schema* table, char* name, enum data_type type) {
    struct column_data column;
    column.name = name;
    column.type = type;
    // somehow append column to the end
}

#define TABLE_OF_TABLES_COL_NUM 3

#define TABLE_OF_TABLES_NAME_COL 1
#define TABLE_OF_TABLES_ID_COL 0
#define TABLE_OF_TABLES_COL_COUNT_COL 2
#define TABLE_OF_TABLES_TABLE_OFFSET 3

#define COLUMN_TABLE_NAME 0
#define COLUMN_TABLE_TABLE_ID 2
#define COLUMN_TABLE_TYPE 1

static void select_from_table(FILE* file, const char* const table_name, char** cols) {

}

/*
Search for a table on several pages in the file and return it if found

*/
static void find_table(FILE* file, const char* const table_name, struct table_schema* table) {
    struct file_header header = {0};
    read_file_header(file, &header);
    void* data = malloc(DEAFULT_PAGE_SIZE);
    struct page_header page_header = {0};
    struct maybe_page current_page = { .exists=true, .offset=header.table_of_tables };
    while(current_page.exists) {
        read_page_binary(file, current_page.offset, data);
        read_page_header(data, &page_header);
        struct item_data rows[page_header.count];
        for (int i = 0; i < page_header.count; ++i) {
            struct column_data col[TABLE_OF_TABLES_COL_NUM];
            rows[i].columns = col;
        }
        parse_page_data(data, page_header, rows);

        for (int i = 0; i < page_header.count; ++i) {
            if (strcmp(rows[i].columns[TABLE_OF_TABLES_NAME_COL].value, table_name) == 0) {
                table->id = *(size_t*)(rows[i].columns[TABLE_OF_TABLES_ID_COL].value);
                table->name = malloc(rows[i].columns[TABLE_OF_TABLES_NAME_COL].length);
                *table->name = *(char*)(rows[i].columns[TABLE_OF_TABLES_NAME_COL].value);
                table->size = *(size_t*)(rows[i].columns[TABLE_OF_TABLES_COL_NUM].value);
                table->next_page = *(size_t*)(rows[i].columns[TABLE_OF_TABLES_TABLE_OFFSET].value);
                table->columns = malloc(sizeof(struct column_data) * table->size);

                void* col_page = malloc(DEAFULT_PAGE_SIZE);
                struct page_header col_page_header = {0};

                struct maybe_page current_col_page = { .exists=true, .offset=header.table_of_columns };
                size_t p = 0;

                while(current_col_page.exists && p < table->size) {
                    read_page_binary(file, current_col_page.offset, col_page);
                    read_page_header(col_page, &col_page_header);
                    struct item_data column_rows[col_page_header.count];
                    for (int i = 0; i < col_page_header.count; ++i) {
                        struct column_data columns[TABLE_OF_TABLES_COL_NUM];
                        column_rows[i].columns = columns;
                    }
                    parse_page_data(col_page, col_page_header, column_rows);

                    for (int i = 0; i < col_page_header.count; ++i) {
                        if (*(size_t*)column_rows[i].columns[COLUMN_TABLE_TABLE_ID].value == table->id) {
                            table->columns[p].name = malloc(column_rows[i].columns[COLUMN_TABLE_NAME].length);
                            *(table->columns[p].name) = *(char*)column_rows[i].columns[COLUMN_TABLE_NAME].value;
                            table->columns[p].type = *(enum data_type*)column_rows[i].columns[COLUMN_TABLE_TYPE].value;
                            p += 1;
                        }   
                    }
                    current_col_page = col_page_header.next_page;
                };
                free(col_page);
                free(data);

                return;
            }   
        }
        current_page = page_header.next_page;
    };
    free(data);
    table = NULL;
    return;
}

/*
Save table to database
table - table to be saved
*/
void save_table(char* file_name, struct table_schema* table) {
    FILE* file = open_file(file_name, "rw+");
    struct table_schema schema = {0};
    find_table(file, table->name, &table);
    if (table != NULL) {
        printf("Table already %s already exists", table->name);
        return;
    }

    struct column columns[TABLE_OF_TABLES_COL_NUM];
    struct item_data row = {columns};

    // create columns for table

    size_t sum_length = 0;

    columns[TABLE_OF_TABLES_NAME_COL].dataType = STRING;
    columns[TABLE_OF_TABLES_NAME_COL].value = table->name;
    columns[TABLE_OF_TABLES_NAME_COL].length = strlen(table->name);

    columns[TABLE_OF_TABLES_ID_COL].dataType = INTEGER;
    columns[TABLE_OF_TABLES_ID_COL].length = sizeof(size_t);
    columns[TABLE_OF_TABLES_ID_COL].value = get_next_id();

    columns[TABLE_OF_TABLES_TABLE_OFFSET].dataType = INTEGER;
    columns[TABLE_OF_TABLES_TABLE_OFFSET].length = sizeof(size_t);
    columns[TABLE_OF_TABLES_TABLE_OFFSET].value = NULL;

    for (int i = 0; i < TABLE_OF_TABLES_COL_NUM; ++i) {
        sum_length += sizeof(enum data_type);
        sum_length += sizeof(size_t);
        sum_length += columns[i].length;
    }

    struct file_header header = {0};
    read_file_header(file, &header);
    void* data = malloc(DEAFULT_PAGE_SIZE);
    struct page_header page_header = {0};
    struct maybe_page current_page = { .exists=true, .offset=header.table_of_tables };

    bool inserted = false;
    while(current_page.exists) {
        read_page_binary(file, current_page.offset, data);
        read_page_header(data, &page_header);

        if (page_header.lower - page_header.upper + 1 < sum_length + sizeof(struct item_id_data)) {
            current_page = page_header.next_page;
            continue;
        }

        void* left = data + page_header.lower;
        for (int i = TABLE_OF_TABLES_COL_NUM - 1; i >= 0; --i) {
            left -= columns[i].length;
            *((char*)left) = *(char*)columns[i].value;
            left -= sizeof(size_t);
            *(char*)(left) = columns[i].length;
            left -= sizeof(enum data_type);
            *(char*)(left) = columns[i].dataType;
        }

        struct item_id_data* right = page_header.upper;
        *right = (struct item_id_data){ .length=TABLE_OF_TABLES_COL_NUM, .offset=left };

        inserted = true;
        
    };
    

    // Add entries to table of columns


    /*
        1. Read table of tables and check whether this table already exists
            1.1 Open file
            1.2 Read file header and find out first page location
            1.3 Read first page starting at known location
            1.4 Parse page to in memory readable format
            1.5 Go throug all entries to this page and try to find name we're trying to add
        2. Add entry to table of table for a new table
            2.1 Find suitable place for new table data
                2.1.1 If this page has enough space use it
                2.1.2 If no place go to the next page and check it
                2.1.3 Repeat until all pages are searched
                2.1.4 if not found place, allocate new page
            2.2 Create item_data block with table columns
            2.3 Save item_data block before page_header.lower
            2.4 Save item_data_id after page_header.upper
            2.5 Update header according to all changes
            2.6 Save page back to the file 
    */
}