#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "user_interface/predicates.h"
#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "util/comparators.h"
#include "user_interface/scanners/scanner_declarations.h"

int64_t __getIntegerFromSelectScanner(void* ptr, char* field);
float __getFloatFromSelectScanner(void* ptr, char* field);
bool __getBoolFromSelectScanner(void* ptr, char* field);
struct String __getStringFromSelectScanner(void* ptr, char* field);
struct Constant __getFieldFromSelectScanner(void* ptr, char* field);

void __setIntegerToSelectScanner(void* ptr, char* field, int64_t value);
void __setFloatToSelectScanner(void* ptr, char* field, float value);
void __setBoolToSelectScanner(void* ptr, char* field, bool value);
void __setStringToSelectScanner(void* ptr, char* field, struct String value);
void __setVarcharToSelectScanner(void* ptr, char* field, char* value);

static bool __selectScanNextFunction(void* ptr);
static void __insertRecordIntoSelectScanner(void* ptr);

void __destroySelectScanner(void* ptr);


struct SelectScanner* createSelectScanner(struct ScanInterface* scan, struct Predicate predicate) {
    struct SelectScanner* scanner = malloc(sizeof(struct SelectScanner));
    scanner->predicate = predicate;
    scanner->tableScanner = scan;

    scanner->scanInterface.goToNextRecord = __selectScanNextFunction;
    scanner->scanInterface.insertNextRecord = __insertRecordIntoSelectScanner;

    scanner->scanInterface.getBool = __getBoolFromSelectScanner;
    scanner->scanInterface.getFloat = __getFloatFromSelectScanner;
    scanner->scanInterface.getInt = __getIntegerFromSelectScanner;
    scanner->scanInterface.getString = __getStringFromSelectScanner;

    scanner->scanInterface.setBool = __setBoolToSelectScanner;
    scanner->scanInterface.setInt = __setIntegerToSelectScanner;
    scanner->scanInterface.setFloat = __setFloatToSelectScanner;
    scanner->scanInterface.setString = __setStringToSelectScanner;
    scanner->scanInterface.setVarchar = __setVarcharToSelectScanner;

    scanner->scanInterface.destroy = __destroySelectScanner;

    return scanner;
}

void __destroySelectScanner(void* ptr) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    scanner->tableScanner->destroy(scanner->tableScanner);
    free(scanner);
}

static bool conditionIsSatisfied(struct SelectScanner* scanner, struct Condition condition) {
    struct Constant constant = getField((struct ScanInterface*)scanner, condition.fieldName);
    switch (constant.type) {
        case INT:
            return compareInts(constant.value.intVal, condition.constant.value.intVal, condition.operator);
        case FLOAT:
            return compareFloats(constant.value.floatVal, condition.constant.value.floatVal, condition.operator);
        case BOOL:
            return compareBools(constant.value.boolVal, condition.constant.value.boolVal, condition.operator);
        case STRING:
            return compareVarchars(constant.value.stringVal, condition.constant.value.stringVal, condition.operator);
    }
    return true;
}

static bool predicateIsSatisfied(struct SelectScanner* scanner, struct Predicate predicate) {
    struct Condition* current = predicate.condition;
    if (current == NULL) {
        return true;
    }

    while (current != NULL) {
        if (!conditionIsSatisfied(scanner, *current)) {
            return false;
        }
        current = current->next;
    };
    return true;
}

int64_t __getIntegerFromSelectScanner(void* ptr, char* field) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    return getInt(scanner->tableScanner, field);
}

float __getFloatFromSelectScanner(void* ptr, char* field) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    return getFloat(scanner->tableScanner, field);
}

bool __getBoolFromSelectScanner(void* ptr, char* field) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    return getBool(scanner->tableScanner, field);
}

struct String __getStringFromSelectScanner(void* ptr, char* field) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    return getString(scanner->tableScanner, field);
}

struct Constant __getFieldFromSelectScanner(void* ptr, char* field) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    return getField(scanner->tableScanner, field);
}

// setters only for update

void __setIntegerToSelectScanner(void* ptr, char* field, int64_t value) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    setInt(scanner->tableScanner, field, value);
}

void __setFloatToSelectScanner(void* ptr, char* field, float value) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    setFloat(scanner->tableScanner, field, value);
}

void __setBoolToSelectScanner(void* ptr, char* field, bool value) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    setBool(scanner->tableScanner, field, value);
}

void __setStringToSelectScanner(void* ptr, char* field, struct String value) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    setString(scanner->tableScanner, field, value);
}

void __setVarcharToSelectScanner(void* ptr, char* field, char* value) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    setVarchar(scanner->tableScanner, field, value);
}

static bool __selectScanNextFunction(void* ptr) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;

    while (scanner->tableScanner->goToNextRecord(scanner->tableScanner)) {
        if (predicateIsSatisfied(scanner, scanner->predicate)) {
            return true;
        }
    }
    return false;
}

static void __insertRecordIntoSelectScanner(void* ptr) {
    struct SelectScanner* scanner = (struct SelectScanner*)ptr;
    insert(scanner->tableScanner);
}