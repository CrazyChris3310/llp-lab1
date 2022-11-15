#include "tests.h"

int main() {
    testReadAndWriteToSinglePage();
    testReadAndWriteToPageFileReopen();
    testDataNotLost();
}