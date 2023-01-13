#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "user_interface/predicates.h"
#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "util/comparators.h"
#include "user_interface/scanners/scanner_declarations.h"

int64_t __getIntegerFromProjectScanner(void* ptr, char* field);
float __getFloatFromProjectScanner(void* ptr, char* field);
bool __getBoolFromProjectScanner(void* ptr, char* field);
struct String __getStringFromProjectScanner(void* ptr, char* field);
struct Constant __getFieldFromProjectScanner(void* ptr, char* field);

void __setIntegerToProjectScanner(void* ptr, char* field, int64_t value);
void __setFloatToProjectScanner(void* ptr, char* field, float value);
void __setBoolToProjectScanner(void* ptr, char* field, bool value);
void __setStringToProjectScanner(void* ptr, char* field, struct String value);
void __setVarcharToProjectScanner(void* ptr, char* field, char* value);

static bool __projectScanNextFunction(void* ptr);
static void __insertRecordIntoProjectScanner(void* ptr);

void __destroyProjectScanner(void* ptr);


struct ProjectScanner* createProjectScanner(struct ScanInterface* scan, struct String* fields) {
    struct ProjectScanner* scanner = malloc(sizeof(struct ProjectScanner));
    scanner->tableScanner = scan;

    scanner->scanInterface.goToNextRecord = __projectScanNextFunction;
    scanner->scanInterface.insertNextRecord = __insertRecordIntoProjectScanner;

    scanner->scanInterface.getBool = __getBoolFromProjectScanner;
    scanner->scanInterface.getFloat = __getFloatFromProjectScanner;
    scanner->scanInterface.getInt = __getIntegerFromProjectScanner;
    scanner->scanInterface.getString = __getStringFromProjectScanner;

    scanner->scanInterface.setBool = __setBoolToProjectScanner;
    scanner->scanInterface.setInt = __setIntegerToProjectScanner;
    scanner->scanInterface.setFloat = __setFloatToProjectScanner;
    scanner->scanInterface.setString = __setStringToProjectScanner;
    scanner->scanInterface.setVarchar = __setVarcharToProjectScanner;

    scanner->scanInterface.destroy = __destroyProjectScanner;

    return scanner;
}

// static bool __hasField(struct ProjectScanner* scanner, char* fieldName) {
//     for (size_t i = 0; i < scanner->fieldCount; i++) {
//         if (strcmp(scanner->fields[i], fieldName) == 0) {
//             return true;
//         }
//     }
//     return false;
// }

// maybe should free array of chars as well
void __destroySelectScanner(void* ptr) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    scanner->tableScanner->destroy(scanner->tableScanner);
    free(scanner);
}

// has fields??????

int64_t __getIntegerFromProjectScanner(void* ptr, char* field) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    return getInt(scanner->tableScanner, field);
}

float __getFloatFromProjectScanner(void* ptr, char* field) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    return getFloat(scanner->tableScanner, field);
}

bool __getBoolFromProjectScanner(void* ptr, char* field) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    return getBool(scanner->tableScanner, field);
}

struct String __getStringFromProjectScanner(void* ptr, char* field) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    return getString(scanner->tableScanner, field);
}

struct Constant __getFieldFromProjectScanner(void* ptr, char* field) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    return getField(scanner->tableScanner, field);
}

// setters only for update

void __setIntegerToProjectScanner(void* ptr, char* field, int64_t value) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    setInt(scanner->tableScanner, field, value);
}

void __setFloatToProjectScanner(void* ptr, char* field, float value) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    setFloat(scanner->tableScanner, field, value);
}

void __setBoolToProjectScanner(void* ptr, char* field, bool value) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    setBool(scanner->tableScanner, field, value);
}

void __setStringToProjectScanner(void* ptr, char* field, struct String value) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    setString(scanner->tableScanner, field, value);
}

void __setVarcharToProjectScanner(void* ptr, char* field, char* value) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    setVarchar(scanner->tableScanner, field, value);
}

static bool __projectScanNextFunction(void* ptr) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    return scanner->tableScanner->goToNextRecord(scanner->tableScanner);
}

static void __insertRecordIntoProjectScanner(void* ptr) {
    struct ProjectScanner* scanner = (struct ProjectScanner*)ptr;
    insert(scanner->tableScanner);
}