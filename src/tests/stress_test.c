#include "middleware/schema.h"
#include "user_interface/read_scan.h"
#include "user_interface/scanners/scanners.h"
#include "user_interface/predicates.h"
#include "user_interface/database.h"
#include "user_interface/query.h"
#include "util/linked_list.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "file_io/file_manager.h"

void insertManyData(struct Database* db, struct InsertQuery* query, size_t n) {
    for (int i = 0; i < n; i++) {
        performInsertQuery(db, query);
    }
}

void testManyInsertions() {
    printf("Testing many insertions\n");
    clock_t start, end;
    double cpu_time_used;

    size_t count = 25;

    double* output = malloc(sizeof(double) * count);

    struct Database* db = openDatabase("database");
    struct Schema* schema = createSchema("table");
    addIntField(schema, "number");

    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(34));

    for (int i = 0; i < count; ++i) {
        dropDatabase(db);
        createTable(db, schema);
    
        insertManyData(db, query, 1000 * i);
        printf("Iteration %d\n", i);

        start = clock();
        
        performInsertQuery(db, query);

        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        output[i] = cpu_time_used;
    }


    destroyInsertQuery(query);
    destroySchema(schema);
    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    fprintf(file, "\ninsert\n");
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%f,", output[i]);
    }
    fclose(file);
    free(output);

    printf("Test successfully finished\n");
}

void testManySelections() {
    printf("Testing many selections\n");
    clock_t start, end;
    double cpu_time_used;

    size_t count = 25;

    double* output = malloc(sizeof(double) * count);

    struct Database* db = openDatabase("database");
    
    struct Schema* schema = createSchema("table");
    addIntField(schema, "number");

    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(34));

    struct SelectQuery* select = createSelectQuery("table", NULL);

    for (int i = 0; i < count; ++i) {
        printf("Iteration %d\n", i);
        dropDatabase(db);
        createTable(db, schema);
        insertManyData(db, query, 1000 * i);

        start = clock();

        struct ScanInterface* scanner = performSelectQuery(db, select);
        while (next(scanner)) {
            assert(getInt(scanner, "number") == 34);
        }

        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        output[i] = cpu_time_used;
        destroyScanner(scanner);
    }

    destroyInsertQuery(query);
    destroySelectQuery(select);
    destroySchema(schema);

    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    fprintf(file, "\nselect\n");
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%f,", output[i]);
    }
    fclose(file);
    free(output);

    printf("Test successfully finished\n");
}

void testDeleteManyData() {
    printf("Testing many deletes\n");
    clock_t start, end;
    double cpu_time_used;

    size_t count = 25;
    double* output = malloc(sizeof(double) * count);

    struct Database* db = openDatabase("database");
    
    struct Schema* schema = createSchema("table");
    addFloatField(schema, "number");

    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(23.6f));

    struct InsertQuery* specific = createInsertQuery("table");
    addInsertionField(specific, "number", constant(48.9f));

    struct Predicate* predicate = createPredicate();
    addCondition(predicate, "number", constant(26.7f), GREATER);
    struct DeleteQuery* delete = createDeleteQuery("table", predicate);

    for (int i = 0; i < count; ++i) {
        dropDatabase(db);
        createTable(db, schema);
        insertManyData(db, query, 1000 * i);
        
        insertManyData(db, specific, 1000);

        start = clock();

        performDeleteQuery(db, delete);

        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        output[i] = cpu_time_used;
    }

    destroyInsertQuery(query);
    destroyInsertQuery(specific);
    destroyDeleteQuery(delete);
    destroySchema(schema);

    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    fprintf(file, "\ndelete\n");
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%f,", output[i]);
    }
    fclose(file);
    free(output);

    printf("Test successfully finished\n");
}

void testUpdateManyData() {
    printf("Testing many updates\n");
    clock_t start, end;
    double cpu_time_used;

    size_t count = 25;
    double* output = malloc(sizeof(double) * count);

    struct Database* db = openDatabase("database");
    
    struct Schema* schema = createSchema("table");
    addFloatField(schema, "number");

    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(23.6f));

    struct InsertQuery* specific = createInsertQuery("table");
    addInsertionField(specific, "number", constant(48.9f));

    struct Predicate* predicate = createPredicate();
    addCondition(predicate, "number", constant(35.7f), GREATER);
    struct UpdateQuery* update = createUpdateQuery("table", "number", constant(45.6f), predicate);

    for (int i = 0; i < count; ++i) {
        dropDatabase(db);
        createTable(db, schema);
        insertManyData(db, query, 1000 * i);
        
        insertManyData(db, specific, 100);

        start = clock();

        performUpdateQuery(db, update);

        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        output[i] = cpu_time_used;
    }

    destroyInsertQuery(query);
    destroyInsertQuery(specific);
    destroyUpdateQuery(update);
    destroySchema(schema);

    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    fprintf(file, "\nupdate\n");
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%f,", output[i]);
    }
    fclose(file);
    free(output);

    printf("Test successfully finished\n");
}

static void printResults(FILE* file, double* output, size_t count, char* name) {
    fprintf(file, "\n%s\n", name);
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%f,", output[i]);
    }
}

void hugeTestThatDoessEverything() {
    printf("Testing everything\n");
    clock_t start, end;
    double cpu_time_used;

    size_t count = 25;
    double* inserts = malloc(sizeof(double) * count);
    double* selects = malloc(sizeof(double) * count);
    double* deletes = malloc(sizeof(double) * count);
    double* updates = malloc(sizeof(double) * count);

    struct Database* db = openDatabase("database");
    
    struct Schema* schema = createSchema("table");
    addFloatField(schema, "number");

    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(23.6f));

    struct InsertQuery* specific = createInsertQuery("table");
    addInsertionField(specific, "number", constant(48.9f));

    struct Predicate* updatePredicate = createPredicate();
    addCondition(updatePredicate, "number", constant(37.586f), GREATER);
    struct UpdateQuery* update = createUpdateQuery("table", "number", constant(37.586f), updatePredicate);

    struct Predicate* selectPredicate = createPredicate();
    addCondition(selectPredicate, "number", constant(37.586f), EQUAL);
    struct SelectQuery* select = createSelectQuery("table", selectPredicate);

    struct Predicate* deletePredicate = createPredicate();
    addCondition(deletePredicate, "number", constant(29.7f), GREATER);
    struct DeleteQuery* delete = createDeleteQuery("table", deletePredicate);

    for (int i = 0; i < count; ++i) {
        printf("Iteration %d\n", i);
        dropDatabase(db);
        createTable(db, schema);
        insertManyData(db, query, 1000 * i);

        start = clock();
        for (int i = 0; i < 1000; ++i) {
            performInsertQuery(db, specific); 
        }
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        inserts[i] = cpu_time_used;

        start = clock();
        performUpdateQuery(db, update);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        updates[i] = cpu_time_used;

        start = clock();
        struct ScanInterface* scan = performSelectQuery(db, select);
        int count = 0;
        while (next(scan)) { 
            assert(compareFloats(getFloat(scan, "number"), 37.586f, EQUAL));
            count += 1;
        }
        assert(count == 1000);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        selects[i] = cpu_time_used;
        destroyScanner(scan);

        start = clock();
        performDeleteQuery(db, delete);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        deletes[i] = cpu_time_used;
    }

    destroyInsertQuery(query);
    destroyInsertQuery(specific);
    destroyDeleteQuery(delete);
    destroySelectQuery(select);
    destroyUpdateQuery(update);
    destroySchema(schema);

    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    printResults(file, inserts, count, "insert");
    printResults(file, selects, count, "select");
    printResults(file, deletes, count, "delete");
    printResults(file, updates, count, "update");
    fclose(file);

    free(inserts);
    free(selects);
    free(deletes);
    free(updates);

    printf("Test successfully finished\n");
}

struct Database {
    struct FileManager* fileManager;
};

void testFileSizeAmongDeletions() {
    printf("Testing file size\n");

    size_t count = 21;
    long* output = malloc(sizeof(long) * count * 2);

    struct Database* db = openDatabase("database");
    dropDatabase(db);
    
    struct Schema* schema = createSchema("table");
    addFloatField(schema, "number");


    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(23.6f));

    struct DeleteQuery* delete = createDeleteQuery("table", NULL);

    for (int i = 0; i < count; ++i) {
        printf("Iteration %d\n", i);
        createTable(db, schema);
        long position_start = getFileLength(db->fileManager);
        insertManyData(db, query, 5000);
        long position_end = getFileLength(db->fileManager);    
        dropTable(db, "table");
        output[i*2] = position_start;
        output[i * 2 + 1] = position_end;
    }

    destroyInsertQuery(query);
    destroyDeleteQuery(delete);
    destroySchema(schema);

    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    fprintf(file, "\nfile_size\n");
    for (int i = 0; i < count * 2; ++i) {
        fprintf(file, "%ld,", output[i]);
    }
    fclose(file);
    free(output);

    printf("Test successfully finished\n");
}

void testFileSizeWithDeleteQuery() {
    printf("Testing file size with delete\n");

    size_t count = 21;
    long* output = malloc(sizeof(long) * count * 2);

    struct Database* db = openDatabase("database");
    dropDatabase(db);
    
    struct Schema* schema = createSchema("table");
    addFloatField(schema, "number");


    struct InsertQuery* query = createInsertQuery("table");
    addInsertionField(query, "number", constant(23.6f));

    struct DeleteQuery* delete = createDeleteQuery("table", NULL);

    for (int i = 0; i < count; ++i) {
        printf("Iteration %d\n", i);
        createTable(db, schema);
        long position_start = getFileLength(db->fileManager);
        insertManyData(db, query, 5000);
        long position_end = getFileLength(db->fileManager);    
        performDeleteQuery(db, delete);
        output[i*2] = position_start;
        output[i * 2 + 1] = position_end;
    }

    destroyInsertQuery(query);
    destroyDeleteQuery(delete);
    destroySchema(schema);

    closeDatabase(db);

    FILE* file = fopen("output.txt", "a");
    fprintf(file, "\nfile_size_delete\n");
    for (int i = 0; i < count * 2; ++i) {
        fprintf(file, "%ld,", output[i]);
    }
    fclose(file);
    free(output);

    printf("Test successfully finished\n");
}