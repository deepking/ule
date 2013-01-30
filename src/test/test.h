#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define run(test) do {\
    printf("----Start %s\n", __FUNCTION__);\
    test();\
    printf("----End   %s\n\n", __FUNCTION__);\
}while (0)

#endif //TEST_H