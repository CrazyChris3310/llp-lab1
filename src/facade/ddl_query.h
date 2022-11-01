#ifndef DDL_QUERY_H
#define DDL_QUERY_H

#include "table_schema.h"

struct table_schema* prepare_table(char* name);
void add_column(struct table_schema* table, char* name, enum data_type type);
void save_table(char* file_name, struct table_schema* table);

#endif 