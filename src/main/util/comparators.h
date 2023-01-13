#ifndef COMPARATORS_H
#define COMPARATORS_H

enum CompareOperator { EQUAL, LESS, GREATER, NOT_EQUAL, GREATER_OR_EQUAL, LESS_OR_EQUAL };

bool compareInts(int64_t a, int64_t b,  enum CompareOperator operator);
bool compareFloats(float a, float b,  enum CompareOperator operator);
bool compareBools(bool a, bool b,  enum CompareOperator operator);
bool compareVarchars(char* a, char* b,  enum CompareOperator operator);

#endif