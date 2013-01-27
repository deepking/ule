#include <string.h>
#include <ctype.h>

#include "test.h"

// testcase suit
extern void test_ts();
extern void test_ule();

void hexdump(const unsigned char *buf, unsigned short len)
{
    char str[80], octet[10];
    int ofs, i, l;

    for (ofs = 0; ofs < len; ofs += 16) {
        sprintf( str, "%03d: ", ofs );

        for (i = 0; i < 16; i++) {
            if ((i + ofs) < len)
                sprintf( octet, "%02x ", buf[ofs + i] );
            else
                strcpy(octet, "   ");

            strcat( str, octet );
        }
        strcat( str, "  " );
        l = strlen( str );

        for (i = 0; (i < 16) && ((i + ofs) < len); i++)
            str[l++] = isprint( buf[ofs + i] ) ? buf[ofs + i] : '.';

        str[l] = '\0';
        printf("%s\n", str);
    }
}

int main(int argc, const char *argv[])
{
    test_ts();
    test_ule();
    return 0;
}
