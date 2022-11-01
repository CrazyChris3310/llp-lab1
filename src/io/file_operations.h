#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "page.h"
#include "operations.h"

#define DEAFULT_PAGE_SIZE 4906

enum operation_status read_page(const char* const file_name, void* data);
enum operation_status write_page(const char* const file_name, const struct page* const page);

#endif