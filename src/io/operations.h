#ifndef OPERATIONS_H
#define OPERATIONS_H

enum operation_status {
    SUCCESS,
    READ_ERROR,
    WRITE_ERROR,
    OPEN_ERROR,
    CLOSE_ERROR,
    SEEK_ERROR,
    ALLOCATION_ERROR
};

#endif
