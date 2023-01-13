#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "middleware/table_manager.h"
#include "middleware/schema.h"
#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "user_interface/scanners/scanners.h"
#include "user_interface/predicates.h"


void tryCreateTables() {
    struct FileManager* fm = createFileManager("database", 4096);
    struct CacheManager* cm = createCacheManager(fm, 10);
    struct TableManager* tm = createTableManager(cm);

    // struct Schema* schema = createSchema("test");
    // addStringField(schema, "name", 30);
    // addIntField(schema, "number");
    // addFloatField(schema, "floating");

    // createTable(tm, schema);

    // destroySchema(schema);
    // schema = NULL;

    struct Schema* schema = findTableSchema(tm, "test");

    struct Field* field = getFieldList(schema); 
    struct Field* dub = field;
    printf("Schema:\n");
    while (dub != NULL) {
        printf("field read: %s - %d\n", dub->name.value, dub->type);
        dub = dub->next;
    }

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

    // destroy(scanner);
    // scanner = NULL;

    // scanner = (struct ScanInterface*)createTableScanner(cm, schema, false, schema->startBlock);


    // struct Schema* schema = findTableSchema(tm, "test");
    // bool isNew = schema->startBlock == -1;

    // struct ScanInterface* scanner = (struct ScanInterface*)createTableScanner(cm, schema, isNew, schema->startBlock);

    // struct Predicate predicate = {0};
    // struct Condition condition = createCondition("name", (struct Constant){ .type=STRING, .value.stringVal="ablubla" }, EQUAL);
    // addCondition(&predicate, condition);

    // struct ScanInterface* selector = (struct ScanInterface*)createSelectScanner(scanner, predicate);

    // printf("\nData:\n");
    // while (next(selector)) {
    //     printf("name: %s\n", getString(selector, "name").value);
    //     printf("number: %" PRId64 "\n", getInt(selector, "number"));
    //     printf("floating: %f\n", getFloat(selector, "floating"));
    // }

    // destroy(selector);

    destroySchema(schema);
    destroyTableManager(tm);
    destroyCacheManager(cm);
    closeFileManager(fm);

}