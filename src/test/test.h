#ifndef TEST_H
#define TEST_H
#include <assert.h>
#include <stdio.h>

#ifndef NDEBUG
#define debug(fmt,args...) printf("[%s:%s:%d] " fmt "\n", __FILE__,__FUNCTION__,__LINE__, ## args)
#else
#define debug(fmt, args...)
#endif

#define info(fmt,args...) printf("[%s:%s:%d] " fmt "\n", __FILE__,__FUNCTION__,__LINE__, ## args)

#define run(test) do {\
    printf("----Start %s\n", __FUNCTION__);\
    test();\
    printf("----End   %s\n\n", __FUNCTION__);\
}while (0)
    

void hexdump(const unsigned char *buf, unsigned short len);

#endif //TEST_H