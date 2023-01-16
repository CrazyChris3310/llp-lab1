#ifndef DATABASE_H
#define DATABASE_H

#include "user_interface/query.h"

struct Database;
struct SelectQuery;
struct Schema;

struct Database* openDatabase(char* filename);
void closeDatabase(struct Database* db);

void dropDatabase(struct Database* database);

void createTable(struct Database* database, struct Schema* schema);
void dropTable(struct Database* database, char* name);
struct ScanInterface* performSelectQuery(struct Database* database, struct SelectQuery* query);
void performInsertQuery(struct Database* database, struct InsertQuery* query);
void performDeleteQuery(struct Database* database, struct DeleteQuery* query);
void performUpdateQuery(struct Database* database, struct UpdateQuery* query);

struct Schema* findTable(struct Database* database, char* tableName);

#endif