#include <stdint.h>
#include <stdlib.h>

#include "util/debug.h"
#include "ule/ts.h"
#include "ule/ule.h"

#include "test.h"

typedef unsigned char byte;

static void test_ule_tx()
{
    SNDUInfo snduinfo;
    byte data[200] = {'h'};
    memset(data, '0', sizeof(data));
    
    ule_init(&snduinfo, IPv4, data, sizeof(data));
    uint32_t totalLength = ule_getTotalLength(&snduinfo);
    unsigned char pkt[totalLength];
    ule_encode(&snduinfo, pkt, totalLength);
    
    debug("sndu: totalLength=%d, Len=%d, type=%xd, dataLen=%d\n", 
        totalLength, snduinfo.length, snduinfo.type, snduinfo.pdu.length);
    hexdump(pkt, totalLength);

    ULEEncapCtx encapCtx;
    ule_initEncapCtx(&encapCtx);
    encapCtx.pid = 0x1FAF;
    encapCtx.snduPkt = pkt;
    encapCtx.snduLen = totalLength;

    ULEDemuxCtx demuxCtx;
    ule_initDemuxCtx(&demuxCtx);
    demuxCtx.pid = 0x1FAF;

    while (encapCtx.snduIndex < encapCtx.snduLen) {
        ule_padding(&encapCtx);
        //hexdump(encapCtx.tsPkt, 188);
        
        ule_demux(&demuxCtx, encapCtx.tsPkt, 188);
        if (demuxCtx.ule_sndu_outbuf) {
            debug("outbuf: len=%d", demuxCtx.ule_sndu_outbuf_len);
            hexdump(demuxCtx.ule_sndu_outbuf, demuxCtx.ule_sndu_outbuf_len);
                        
            // clean & reset outbuf
            free(demuxCtx.ule_sndu_outbuf);
            demuxCtx.ule_sndu_outbuf = NULL;
            demuxCtx.ule_sndu_outbuf_len = 0;
        }
    }
}

void test_ule()
{
    run(test_ule_tx);
}