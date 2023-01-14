#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

enum DataType { INT, FLOAT, STRING, BOOL};

struct Constant {
    enum DataType type;
    union {
        int64_t intVal;
        float floatVal;
        bool boolVal;
        char* stringVal;
    } value;
};

struct Constant intConstant(int64_t value) {
    return (struct Constant){ .type = INT, .value.intVal = value };
}

struct Constant floatConstant(float value) {
    return (struct Constant){ .type = FLOAT, .value.floatVal = value };
}

struct Constant boolConstant(bool value) {
    return (struct Constant){ .type = BOOL, .value.boolVal = value };
}

struct Constant stringConstant(char* value) {
    return (struct Constant){ .type = STRING, .value.stringVal = value };
}

#define constant(x) _Generic((x), \
    int:  intConstant, \
    float:   floatConstant, \
    char*: stringConstant, \
    bool: boolConstant)(x);

int main() {

    struct Constant cnst = constant(123);
    struct Constant two = constant(2.0f);
    struct Constant three = constant("fingerprig");
    struct Constant four = constant(true);

    printf("Type: %d, value: %" PRId64 "\n", cnst.type, cnst.value.intVal);
    printf("Type: %d, value: %f\n", two.type, two.value.floatVal);
    printf("Type: %d, value: %s\n", three.type, three.value.stringVal);
    printf("Type: %d, value: %d\n", four.type, four.value.boolVal);
}