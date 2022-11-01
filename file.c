#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum data_type { INTEGER, FLOAT, STRING, BOOLEAN };

struct huy {
    union {
        char* string;
        float* floating;
        int* integer;
        bool* boolean;
    } val;
};

#define typename(x) _Generic((x), \
    int:     INTEGER, \
    float:   FLOAT, \
    char*: STRING, \
    default: BOOLEAN)

int main() {
    void* astra = malloc(45);
    struct huy* a = malloc(sizeof(struct huy));
    *(int*)astra = 1;
    a->val.boolean = astra;

    printf("%s\n", a->val.string);
    printf("%f\n", *a->val.floating);
    printf("%d\n", *a->val.integer);
    printf("%d\n", *a->val.boolean);

    free(a);
    free(astra);

    return 0;
}