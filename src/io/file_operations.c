#include "operations.h"
#include "page.h"
#include <stdio.h>
#include "file_operations.h"

static enum operation_status open_file(FILE** f, const char* const filename, const char* const mode) {
    *f = fopen(filename, mode);
    if (*f == NULL) {
        return OPEN_ERROR;
    }
    return SUCCESS;
}

static enum operation_status close_file(FILE* f) {
    const int result = fclose(f);
    if (result != 0) {
        return CLOSE_ERROR;
    }
    return SUCCESS;
}

struct file_handle* open_file(const char* const filename, const char* const mode) {
    struct file_handle* file = malloc(sizeof(struct file_handle));
    // TODO: handle exceptions and statuses
    open_file(file->file, filename, mode);
    read_file_header(file->file, &file->header);
    return file;
}

static enum operation_status read_file_header(FILE* file, struct file_header* header) {
    fseek(file, 0, SEEK_SET);
    size_t read = fread(header, sizeof(struct file_header), 1, file);
    if (read != 1) {
        return READ_ERROR;
    }
    return SUCCESS;
}

static enum operation_status read_page_binary(FILE* file, uint16_t offset, void* data) {
    fseek(file, offset, SEEK_SET);
    size_t read = fread(data, DEAFULT_PAGE_SIZE, 1, file);
    if (read != DEAFULT_PAGE_SIZE) {
        return READ_ERROR;
    }
    return SUCCESS;
}

static void parse_page_header(void* data, struct page_header* header) {
    *header = *(struct page_header*)data;
}

static void read_column(const void* const start, struct column* col) {
    col->dataType = *(enum data_type*)start;
    col->length = *(uint16_t*)start + sizeof(enum data_type);
    col->value = start + sizeof(enum data_type) + sizeof(uint16_t);
}

enum operation_status parse_page_data(void* data, struct page_header header, struct item_data* buf) {
    struct item_id_data ids[] = data + sizeof(struct page_header);
    for (int i = 0; i < header.count; ++i) {
        size_t came = data + ids[i].offset;
        for (int j = 0; j < ids[i].length; ++j) {
            read_column(came, &buf[i].columns[j]);
            came += sizeof(enum data_type) + sizeof(uint16_t) + buf[i].columns[j].length;
        }
    }
}
