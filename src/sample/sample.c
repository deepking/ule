#include <stdio.h>
#include <stdlib.h>

extern int tx(unsigned char handleNum);
extern int rx(unsigned char handleNum);

int main(int argc, char** argv)
{
    unsigned char handleNum;
    unsigned char proto;
    
    if(argv[1]==NULL){
        printf("\n\n================ Open default device handle ==================\n");
        printf("= To chose another driver handle. Please input handle number =\n");		
        printf("= Example: %s 1 0 -> 1 for usb-it950x1 handle\n", argv[0]);
        printf("= second: 0 tx mode, 1 rx mode ===\n");
        printf("==============================================================\n");
        handleNum = 0;
    } else {
        handleNum = atoi(argv[1]);
        if(atoi(argv[1]) < 0) {
            printf("\n=============== The bad handle number! Please input again! =============\n\n");
            printf("\n===================== To chose driver handle sample ====================\n");
            printf("======= Example: ./testkit_it950x_tx   -> for usb-it950x0 handle =======\n");			
            printf("======= Example: ./testkit_it950x_tx 1 -> for usb-it950x1 handle =======\n");
            printf("========================================================================\n");
            return 0;
        }
    }
    
    proto = atoi(argv[2]);
    switch (proto) {
        case 0: return tx(handleNum);
        case 1: return rx(handleNum);
        default: break;
    }
    
    return 0;
}
