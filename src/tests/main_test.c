#include "user_interface/scanners/scanners.h"
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "middleware/table_manager.h"
#include "middleware/schema.h"

void tryCreateTables() {
    struct FileManager* fm = createFileManager("database", 4096);
    struct CacheManager* cm = createCacheManager(fm, 10);
    struct TableManager* tm = createTableManager(cm);

    struct Schema* schema = createSchema("test");
    addStringField(schema, "name", 30);
    addIntField(schema, "number");
    addFloatField(schema, "floating");

    createTable(tm, schema);

    destroySchema(schema);
    schema = NULL;

    schema = findTableSchema(tm, "test");

    struct Field* field = getFieldList(schema); 
    struct Field* dub = field;
    printf("Schema:\n");
    while (dub != NULL) {
        printf("field read: %s - %d\n", dub->name.value, dub->type);
        dub = dub->next;
    }

    bool isNew = schema->startBlock == -1;
    struct TableScanner* scanner = createTableScanner(cm, schema, isNew, schema->startBlock);

    insert(scanner);
    setVarchar(scanner, "name", "ablubla");
    setInt(scanner, "number", 123);
    setFloat(scanner, "floating", 123.456);

    insert(scanner);
    setVarchar(scanner, "name", "it's done!");
    setInt(scanner, "number", 29571234);
    setFloat(scanner, "floating", 544.178);

    destroyTableScanner(scanner);
    scanner = NULL;

    printf("\nData:\n");

    scanner = createTableScanner(cm, schema, false, schema->startBlock);
    while (next(scanner)) {
        printf("name: %s\n", getString(scanner, "name").value);
        printf("number: %" PRId64 "\n", getInt(scanner, "number"));
        printf("floating: %f\n", getFloat(scanner, "floating"));
    }

    destroyTableScanner(scanner);

    destroySchema(schema);
    destroyTableManager(tm);
    destroyCacheManager(cm);
    closeFileManager(fm);

}