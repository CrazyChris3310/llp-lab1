#ifndef SCANNERS_H
#define SCANNERS_H

#include <inttypes.h>
#include <stdbool.h>
#include "middleware/schema.h"
#include "page_cacheing/cache_manager.h"

struct ScanInterface;
struct TableScanner;
struct SelectScanner;

struct TableScanner* createTableScanner(struct CacheManager* cm, struct Schema* schema, bool isNew, size_t blockStart);
size_t getCurrentBlock(struct TableScanner* scanner);

struct Predicate;

struct SelectScanner* createSelectScanner(struct ScanInterface* scan, struct Predicate predicate);
struct JoinScanner* createJoinScanner(struct ScanInterface* left, struct ScanInterface* right);


#endif