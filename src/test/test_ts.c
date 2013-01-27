#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "test.h"
#include "ule/ts.h"

void test_ts()
{
    unsigned char buf[188] = {0};
    ts_reset(buf);
    hexdump(buf, 188);
    printf("isValidTSPacket: %d\n", ts_isValidPacket(buf));
    assert(ts_isValidPacket(buf));

    printf("PUSI: %d\n", ts_getPUSI(buf));
    assert(!ts_getPUSI(buf));

    printf("PID: %x\n", ts_getPID(buf));
    assert(ts_getPID(buf) == 0x1FFF);

    ts_setPID(buf, 1000);
    printf("PID: %x\n", ts_getPID(buf));

    printf("Counter: %d\n", ts_getContinuityCounter(buf));
    assert(ts_getContinuityCounter(buf) == 0);

}



