#include <inttypes.h>
#include <stdio.h>

int main() {
    size_t number = *(size_t*)(NULL);
    printf("%zu", number);
    return 0;
}