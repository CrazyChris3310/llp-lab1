#ifndef DATABASE_H
#define DATABASE_H

struct Database;
struct SelectQuery;
struct Schema;

struct Database* openDatabase(char* filename);
void closeDatabase(struct Database* db);

void createTable(struct Database* database, struct Schema* schema);
struct ScanInterface* performQuery(struct Database* database, char* tableName, struct SelectQuery* query);

#endif