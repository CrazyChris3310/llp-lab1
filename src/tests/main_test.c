#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "middleware/table_manager.h"
#include "middleware/schema.h"
#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "user_interface/scanners/scanners.h"
#include "user_interface/predicates.h"
#include "user_interface/database.h"
#include "user_interface/query.h"


void tryCreateTables() {
    
    struct Database* database = openDatabase("database");

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(database, schema);

    destroySchema(schema);
    schema = NULL;

    // schema = findTableSchema(tm, "test");

    // struct Field* field = getFieldList(schema); 
    // struct Field* dub = field;
    // printf("Schema:\n");
    // while (dub != NULL) {
    //     printf("field read: %s - %d\n", dub->name.value, dub->type);
    //     dub = dub->next;
    // }

    // bool isNew = schema->startBlock == -1;
    // struct ScanInterface* scanner = (struct ScanInterface*)createTableScanner(cm, schema, isNew, schema->startBlock);


    // insert(scanner);
    // setVarchar(scanner, "name", "ablubla");
    // setInt(scanner, "number", 123);
    // setFloat(scanner, "floating", 123.456);

    // insert(scanner);
    // setVarchar(scanner, "name", "it's done!");
    // setInt(scanner, "number", 29571234);
    // setFloat(scanner, "floating", 544.178);

    // insert(scanner);
    // setVarchar(scanner, "name", "finger toes");
    // setInt(scanner, "number", 123);
    // setFloat(scanner, "floating", 987.654);

    // insert(scanner);
    // setVarchar(scanner, "name", "mistica");
    // setInt(scanner, "number", 543);
    // setFloat(scanner, "floating", 987.654);

    // insert(scanner);
    // setVarchar(scanner, "name", "ablubla");
    // setInt(scanner, "number", 287);
    // setFloat(scanner, "floating", 987.654);

    // destroySchema(schema);
    // schema = NULL;
    // scanner = NULL;

    struct Predicate* predicate = createPredicate();
    struct Condition condition = createCondition("number", (struct Constant){ .type=INT, .value.intVal=123}, EQUAL);
    addCondition(predicate, condition);

    struct SelectQuery query = createSelectQuery("test", predicate);

    struct ScanInterface* scanner = performSelectQuery(database, &query);
    printf("\nData:\n");
    while (next(scanner)) {
        printf("name: %s\n", getString(scanner, "name").value);
        printf("number: %" PRId64 "\n", getInt(scanner, "number"));
        printf("floating: %f\n", getFloat(scanner, "floating"));
    }

    destroyScanner(scanner);
    destroyPredicate(predicate);

    closeDatabase(database);

}