#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "util/comparators.h"

bool compareInts(int64_t a, int64_t b,  enum CompareOperator operator) {
    switch (operator) {
        case EQUAL:
            return a == b;
        case NOT_EQUAL:
            return a != b;
        case GREATER:
            return a > b;
        case GREATER_OR_EQUAL:
            return a >= b;
        case LESS:
            return a < b;
        case LESS_OR_EQUAL:
            return a <= b;
    }
    return false;
}

const float accuracy = 1e-5;

bool compareFloats(float a, float b,  enum CompareOperator operator) {
    switch (operator) {
        case EQUAL:
            return fabsf(a - b) < accuracy;
        case GREATER:
            return a > b;
        case GREATER_OR_EQUAL:
            return a >= b;
        case LESS:
            return a < b;
        case LESS_OR_EQUAL:
            return a <= b;
        default:
            return fabsf(a - b) >= accuracy;
    }
}

bool compareBools(bool a, bool b,  enum CompareOperator operator) {
    switch (operator) {
        case EQUAL:
            return a == b;
        case NOT_EQUAL:
            return a != b;
        default:
            return false;
    }
}

bool compareVarchars(char* a, char* b,  enum CompareOperator operator) {
    int res = strcmp(a, b);
    switch (operator) {
        case EQUAL:
            return res == 0;
        case NOT_EQUAL:
            return res != 0;
        case GREATER:
            return res > 0;
        case GREATER_OR_EQUAL:
            return res >= 0;
        case LESS:
            return res < 0;
        case LESS_OR_EQUAL:
            return res <= 0;
    }
    return false;
}