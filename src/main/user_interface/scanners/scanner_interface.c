#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include "util/my_string.h"
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "middleware/page_record.h"
#include "scanner_declarations.h"

int64_t getInt(struct TableScanner* scanner, char* field) {
    return scanner->getInt(scanner, field);
}
float getFloat(struct TableScanner* scanner, char* field) {
    return scanner->getFloat(scanner, field);
}
bool getBool(struct TableScanner* scanner, char* field) {
    return scanner->getBool(scanner, field);
}
struct String getString(struct TableScanner* scanner, char* field) {
    return scanner->getString(scanner, field);
}

void setInt(struct TableScanner* scanner, char* field, int64_t value) {
    scanner->setInt(scanner, field, value);
}

void setFloat(struct TableScanner* scanner, char* field, float value) {
    scanner->setFloat(scanner, field, value);
}

void setBool(struct TableScanner* scanner, char* field, bool value) {
    scanner->setBool(scanner, field, value);
}

void setString(struct TableScanner* scanner, char* field, struct String value) {
    scanner->setString(scanner, field, value);
}

void setVarchar(struct TableScanner* scanner, char* field, char* value) {
    scanner->setVarchar(scanner, field, value);
}

bool next(struct TableScanner* scanner) {
    return scanner->goToNextRecord(scanner);
}

void insert(struct TableScanner* scanner) {
    scanner->insertNextRecord(scanner);
}