#ifndef DATA_TYPES_H
#define DATA_TYPES_H

enum DataType { INT, FLOAT, STRING, BOOL };

#define typename(x) _Generic((x), \
    int:  INT, \
    float:   FLOAT, \
    char*: STRING, \
    bool: BOOL)

#endif