#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include "util/my_string.h"
#include "file_io/file_manager.h"
#include "page_cacheing/cache_manager.h"
#include "middleware/page_record.h"
#include "scanner_declarations.h"

#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "user_interface/predicates.h"

int64_t getInt(struct ScanInterface* scanner, char* field) {
    return scanner->getInt(scanner, field);
}
float getFloat(struct ScanInterface* scanner, char* field) {
    return scanner->getFloat(scanner, field);
}
bool getBool(struct ScanInterface* scanner, char* field) {
    return scanner->getBool(scanner, field);
}
struct String getString(struct ScanInterface* scanner, char* field) {
    return scanner->getString(scanner, field);
}

void setInt(struct ScanInterface* scanner, char* field, int64_t value) {
    scanner->setInt(scanner, field, value);
}

void setFloat(struct ScanInterface* scanner, char* field, float value) {
    scanner->setFloat(scanner, field, value);
}

struct Constant getField(struct ScanInterface* scanner, char* field) {
    return scanner->getField(scanner, field);
}

void setBool(struct ScanInterface* scanner, char* field, bool value) {
    scanner->setBool(scanner, field, value);
}

void setString(struct ScanInterface* scanner, char* field, struct String value) {
    scanner->setString(scanner, field, value);
}

void setVarchar(struct ScanInterface* scanner, char* field, char* value) {
    scanner->setVarchar(scanner, field, value);
}

bool next(struct ScanInterface* scanner) {
    return scanner->goToNextRecord(scanner);
}

void insert(struct ScanInterface* scanner) {
    scanner->insertNextRecord(scanner);
}

void destroy(struct ScanInterface* scanner) {
    scanner->destroy(scanner);
}