#include "tests.h"
#include <stdio.h>

int main() {
    printf("\n\n------------------Simple tests------------------\n\n");
    testCreateTable();
    testInsertAndReadData();
    testMultipleInsert();
    testSelectWithCondition();
    testDeleteData();
    testDeleteAllData();
    testUpdateData();
    testJoinSelect();

    // printf("\n\n------------------Stress tests------------------\n\n");

    // fclose(fopen("output.txt", "w"));

    // // hugeTestThatDoessEverything();

    // testManyInsertions();
    // testManySelections();
    // testDeleteManyData();
    // testUpdateManyData();
}