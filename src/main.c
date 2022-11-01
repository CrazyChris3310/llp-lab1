#include "ddl_query.h"

int main() {
    char* file_name = "data.storage";
    struct table_schema* table = prepare_table("user");
    add_column(table, "id", INTEGER);
    add_column(table, "name", STRING);
    add_column(table, "age", INTEGER);
    save_table(file_name, table);
    free(table);

    table = prepare_table("thing");
    add_column(table, "name", STRING);
    add_column(table, "owner", INTEGER);
    save_table(file_name, table);
    free(table);
}