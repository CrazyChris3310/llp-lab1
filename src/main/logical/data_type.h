#ifndef DATA_TYPES_H
#define DATA_TYPES_H

enum DataType { INTEGER, FLOAT, STRING, BOOLEAN };

#define typename(x) _Generic((x), \
    int:  INTEGER, \
    float:   FLOAT, \
    char*: STRING, \
    bool: BOOLEAN)

#endif