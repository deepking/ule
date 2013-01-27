#include <stdint.h>

#include "test.h"
#include "ule/ts.h"
#include "ule/ule.h"

typedef unsigned char byte;

static void test_ule_tx()
{
    SNDUInfo info;
    byte data[173] = {'h'};
    ule_init(&info, IPv4, data, 173);
    uint32_t totalLength = ule_getTotalLength(&info);
    unsigned char pkt[totalLength];
    ule_encode(&info, pkt, totalLength);
    debug("%d\n", totalLength);

    ULEEncapCtx ctx;
    ule_initEncapCtx(&ctx);
    ctx.pid = 0x1FAF;
    ctx.snduPkt = pkt;
    ctx.snduLen = totalLength;

    while (ctx.snduIndex < ctx.snduLen) {
        ule_padding(&ctx);
        hexdump(ctx.tsPkt, 188);
    }
}

void test_ule()
{
    run(test_ule_tx);
}