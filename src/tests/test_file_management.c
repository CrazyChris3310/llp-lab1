#include <assert.h>
#include <inttypes.h>
#include "file_io/file_manager.h"
#include "file_io/page.h"
#include "util/my_string.h"

#define DEAFULT_PAGE_SIZE 4096

void testReadAndWriteToSinglePage() {
    struct FileManager* fm = createFileManager("database", DEAFULT_PAGE_SIZE);
    struct Page* page = allocatePage(fm->blockSize);

    size_t position = 68;
    struct String str = { 12, "sliva spelaya" };
    setPageString(page, position, str);
    size_t pos2 = position + str.lenght + sizeof(int64_t);
    setPageInt(page, pos2, 794);
    writePage(fm, 2, page);
    deallocatePage(page);

    page = allocatePage(fm->blockSize);
    readPage(fm, 2, page);
    assert(getPageInt(page, pos2) == 794);
    assert(compareStrings(getPageString(page, position), str) == 0);
    printf("String read: %s\n", getPageString(page, position).value);
    printf("Integer read: %" PRId64 "\n", getPageInt(page, pos2));
    deallocatePage(page);
    closeFileManager(fm);
}

void testReadAndWriteToPageFileReopen() {
    struct FileManager* fm = createFileManager("database", DEAFULT_PAGE_SIZE);
    struct Page* page = allocatePage(fm->blockSize);

    size_t position = 68;
    struct String str = { 12, "sliva spelaya" };
    setPageString(page, position, str);
    size_t pos2 = position + str.lenght + sizeof(int64_t);
    setPageInt(page, pos2, 794);
    writePage(fm, 2, page);
    deallocatePage(page);
    closeFileManager(fm);

    fm = createFileManager("database", DEAFULT_PAGE_SIZE);
    page = allocatePage(fm->blockSize);

    readPage(fm, 2, page);
    assert(getPageInt(page, pos2) == 794);
    assert(compareStrings(getPageString(page, position), str) == 0);
    deallocatePage(page);
    closeFileManager(fm);
}