#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "user_interface/predicates.h"
#include "user_interface/read_scan.h"
#include "user_interface/write_scan.h"
#include "util/comparators.h"
#include "user_interface/scanners/scanner_declarations.h"
#include "util/linked_list.h"

bool __joinScannerHasField(void* ptr, char* field);
int64_t __getIntegerFromJoinScanner(void* ptr, char* field);
float __getFloatFromJoinScanner(void* ptr, char* field);
bool __getBoolFromJoinScanner(void* ptr, char* field);
struct String __getStringFromJoinScanner(void* ptr, char* field);
struct Constant __getFieldFromJoinScanner(void* ptr, char* field);

void __setIntegerToJoinScanner(void* ptr, char* field, int64_t value);
void __setFloatToJoinScanner(void* ptr, char* field, float value);
void __setBoolToJoinScanner(void* ptr, char* field, bool value);
void __setStringToJoinScanner(void* ptr, char* field, struct String value);
void __setVarcharToJoinScanner(void* ptr, char* field, char* value);

static bool __joinScanNextFunction(void* ptr);

void __joinInsertion(void* ptr);
void __destroyJoinScanner(void* ptr);
static void __resetJoinScanner(void* ptr);


struct JoinScanner* createJoinScanner(struct ScanInterface* left, struct ScanInterface* right) {
    struct JoinScanner* scanner = malloc(sizeof(struct JoinScanner));
    scanner->leftScanner = left;
    scanner->rightScanner = right;
    next(left);

    scanner->scanInterface.goToNextRecord = __joinScanNextFunction;
    scanner->scanInterface.insertNextRecord = __joinInsertion;

    scanner->scanInterface.hasField = __joinScannerHasField;
    scanner->scanInterface.getBool = __getBoolFromJoinScanner;
    scanner->scanInterface.getFloat = __getFloatFromJoinScanner;
    scanner->scanInterface.getInt = __getIntegerFromJoinScanner;
    scanner->scanInterface.getString = __getStringFromJoinScanner;
    scanner->scanInterface.getField = __getFieldFromJoinScanner;

    scanner->scanInterface.setBool = __setBoolToJoinScanner;
    scanner->scanInterface.setInt = __setIntegerToJoinScanner;
    scanner->scanInterface.setFloat = __setFloatToJoinScanner;
    scanner->scanInterface.setString = __setStringToJoinScanner;
    scanner->scanInterface.setVarchar = __setVarcharToJoinScanner;

    scanner->scanInterface.destroy = __destroyJoinScanner;
    scanner->scanInterface.reset = __resetJoinScanner;
    scanner->scanInterface.deleteRecord = __joinInsertion;

    return scanner;
}

void __destroyJoinScanner(void* ptr) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    scanner->leftScanner->destroy(scanner->leftScanner);
    scanner->rightScanner->destroy(scanner->rightScanner);
    free(scanner);
}

void __resetJoinScanner(void* ptr) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    scanner->leftScanner->reset(scanner->leftScanner);
    next(scanner->leftScanner);
    scanner->rightScanner->reset(scanner->rightScanner);
}

bool __joinScannerHasField(void* ptr, char* field) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    return scanner->leftScanner->hasField(scanner->leftScanner, field) || scanner->rightScanner->hasField(scanner->rightScanner, field);
}

int64_t __getIntegerFromJoinScanner(void* ptr, char* field) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    if (scanner->leftScanner->hasField(scanner->leftScanner, field)) {
        return getInt(scanner->leftScanner, field);
    } else {
        return getInt(scanner->rightScanner, field);
    }
}

float __getFloatFromJoinScanner(void* ptr, char* field) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    if (scanner->leftScanner->hasField(scanner->leftScanner, field)) {
        return getFloat(scanner->leftScanner, field);
    } else {
        return getFloat(scanner->rightScanner, field);
    }
}

bool __getBoolFromJoinScanner(void* ptr, char* field) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    if (scanner->leftScanner->hasField(scanner->leftScanner, field)) {
        return getBool(scanner->leftScanner, field);
    } else {
        return getBool(scanner->rightScanner, field);
    }
}

struct String __getStringFromJoinScanner(void* ptr, char* field) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    if (scanner->leftScanner->hasField(scanner->leftScanner, field)) {
        return getString(scanner->leftScanner, field);
    } else {
        return getString(scanner->rightScanner, field);
    }
}

struct Constant __getFieldFromJoinScanner(void* ptr, char* field) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;
    if (scanner->leftScanner->hasField(scanner->leftScanner, field)) {
        return getField(scanner->leftScanner, field);
    } else {
        return getField(scanner->rightScanner, field);
    }
}

void __setIntegerToJoinScanner(void* ptr, char* field, int64_t value) {}

void __setFloatToJoinScanner(void* ptr, char* field, float value) {}

void __setBoolToJoinScanner(void* ptr, char* field, bool value) {}

void __setStringToJoinScanner(void* ptr, char* field, struct String value) {}

void __setVarcharToJoinScanner(void* ptr, char* field, char* value) {}

static bool __joinScanNextFunction(void* ptr) {
    struct JoinScanner* scanner = (struct JoinScanner*)ptr;

    if (next(scanner->rightScanner)) {
        return true;
    } else {
        reset(scanner->rightScanner);
        return next(scanner->rightScanner) && next(scanner->leftScanner);
    }
}

void __joinInsertion(void* ptr) {}