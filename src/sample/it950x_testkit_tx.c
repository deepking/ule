//
// This is a sample testkit for IT950x device (modulator).
//
//#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "driver/api.h"
#include "util/crc.h"
#include "util/debug.h"
#include "ule/ule.h"

#define LIST_SIZE(x) sizeof(x)/sizeof(Param)
#define TX_DATA_LEN 65424


typedef struct {
    char *name;
    int value;
} Param;

typedef struct {
    Dword Frequency;
    Word Bandwidth;
} ModulatorParam;

typedef struct {
    uint32_t count;
    uint32_t ulErrCount;
    uint32_t ulLostCount;
    uint16_t pid;
    uint8_t  sequence1;
    uint8_t  sequence2;
    uint8_t  dup_flag;
} PIDINFO, *PPIDINFO;

//static int gTransferRate = 0;
static uint32_t gTransferInterval = 0;
static int gRateControl = 0; 
static Dword g_ChannelCapacity = 0;
static Byte NullPacket[188]={0x47,0x1f,0xff,0x1c,0x00,0x00};
static MODULATION_PARAM g_ChannelModulation_Setting;

//static int peek_character = -1;  
//static struct termios initial_settings, new_settings;  

int kbhit(void)  
{  
    struct termios oldt, newt;  
    int ch;  
    int oldf;  
    tcgetattr(STDIN_FILENO, &oldt);  
    newt = oldt;  
    newt.c_lflag &= ~(ICANON | ECHO);  
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);  
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);  
    ch = getchar();  
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
    fcntl(STDIN_FILENO, F_SETFL, oldf);  
    if(ch != EOF)  
    {  
        ungetc(ch, stdin);  
        return 1;  
    }  
    return 0;  
}  

static int GetDriverInfo(Byte handleNum)
{
    uint32_t ChipType = 0;		
    Byte Tx_NumOfDev = 0;
    TxDemodDriverInfo driverInfo;
    Dword dwError = ERR_NO_ERROR;

    if((dwError = g_ITEAPI_TxGetNumOfDevice(&Tx_NumOfDev)) == ERR_NO_ERROR)
        printf("%d Devices\n", Tx_NumOfDev);
    else 
        printf("g_ITEAPI_TxGetNumOfDevice error\n");	

    if((dwError = g_ITEAPI_TxDeviceInit(handleNum)) == ERR_NO_ERROR)
        printf("g_ITEAPI_TxDeviceInit ok\n");
    else 
        printf("g_ITEAPI_TxDeviceInit fail\n");

    if((dwError = g_ITEAPI_TxLoadIQTableFromFile()) == ERR_NO_ERROR)
        printf("g_ITEAPI_TxLoadIQTableFromFile ok\n");
    else
        printf("g_ITEAPI_TxLoadIQTableFromFile fail\n");		

    if((dwError = g_ITEAPI_TxGetChipType(&ChipType)) == ERR_NO_ERROR)
        printf("g_ITE_TxGetChipType ok\n");
    else
        printf("g_ITE_TxGetChipType fail\n");

    if ((dwError = g_ITEAPI_GetDrvInfo(&driverInfo))) {
        printf("Get Driver Info failed 0x%lu!\n", dwError);
    }
    else {
        printf("g_ITEAPI_GetDrvInfo ok\n");		
        printf("DriverVerion  = %s\n", driverInfo.DriverVerion);
        printf("APIVerion     = %s\n", driverInfo.APIVerion);
        printf("FWVerionLink  = %s\n", driverInfo.FWVerionLink);
        printf("FWVerionOFDM  = %s\n", driverInfo.FWVerionOFDM);
        printf("Company       = %s\n", driverInfo.Company);
        printf("SupportHWInfo = %s\n", driverInfo.SupportHWInfo);
        printf("ChipType      = 0x%x", ChipType);
    }

    return dwError;
}

static long TX_SetChannelTransmissionParameters(ModulatorParam *param)
{
    Dword tempFreq = 0;
    Word tempBw = 0;
    int temp, ret;
    Dword dwStatus = 0;
    Dword ChannelCapacity = 0;	
    MODULATION_PARAM ChannelModulation_Setting;	

    printf("\n=> Please Input Frequency in KHz (ex. 666000KHz): ");
    scanf("%d", &temp);
    tempFreq = temp;

    printf("\n=> Please Input Bandwidth in KHz (ex. 8000KHz): ");
    ret = scanf("%d",&temp);
    tempBw = temp;

    dwStatus = g_ITEAPI_TxSetChannel(tempFreq, tempBw);
    if (dwStatus) {
        printf("g_ITEAPI_TxSetChannel error\n");	
        return dwStatus;
    }
    else {
        printf("g_ITEAPI_TxSetChannel ok\n");
    }

    param->Frequency = tempFreq;
    param->Bandwidth = tempBw;

    printf("\n=> Please Input constellation (0:QPSK  1:16QAM  2:64QAM): ");
    ret = scanf("%d", &temp);
    ChannelModulation_Setting.constellation = (Byte) temp;

    printf("\n=> Please Input Code Rate");
    printf(" (0:1/2  1:2/3  2:3/4  3:5/6  4:7/8): ");
    ret = scanf("%d", &temp);
    ChannelModulation_Setting.highCodeRate = (Byte)temp;

    printf("\n=> Please Input Interval (0:1/32  1:1/16  2:1/8  3:1/4): ");
    ret = scanf("%d", &temp);
    ChannelModulation_Setting.interval = (Byte)temp;

    printf("\n=> Please Input Transmission Mode (0:2K  1:8K): ");
    ret = scanf("%d", &temp);
    ChannelModulation_Setting.transmissionMode = (Byte)temp;	

    printf("\nFrequency = %lu KHz\n", tempFreq);
    printf("Bandwidth = %d MHz\n", tempBw);

    switch(ChannelModulation_Setting.constellation) {
    case 0: printf("Constellation: QPSK\n"); break;
    case 1: printf("Constellation: 16QAM\n"); break;			
    case 2: printf("Constellation: 64QAM\n"); break;	
    default: printf("Input Constellation Error\n"); return ERR_INVALID_INDEX;
    }	
    switch(ChannelModulation_Setting.highCodeRate) {
    case 0: printf("Code Rate: 1/2\n"); break;
    case 1: printf("Code Rate: 2/3\n"); break;			
    case 2: printf("Code Rate: 3/4\n"); break;	
    case 3: printf("Code Rate: 5/6\n"); break;			
    case 4: printf("Code Rate: 7/8\n"); break;	
    default: printf("Input Code Rate Error\n");	return ERR_INVALID_INDEX;
    }	
    switch(ChannelModulation_Setting.interval) {
    case 0: printf("Interval: 1/32\n"); break;
    case 1: printf("Interval: 1/16\n"); break;			
    case 2: printf("Interval: 1/8\n"); break;	
    case 3: printf("Interval: 1/4\n"); break;			
    default: printf("Input Interval Error\n");	return ERR_INVALID_INDEX;
    }		
    switch(ChannelModulation_Setting.transmissionMode) {
    case 0: printf("Transmission Mode: 2K\n"); break;
    case 1: printf("Transmission Mode: 8K\n"); break;			
    default: printf("Input Transmission Mode Error\n");	return ERR_INVALID_INDEX;
    }	

    dwStatus = g_ITEAPI_TxSetChannelModulation(ChannelModulation_Setting);//transmissionMode, constellation, interval, highCodeRate);

    if (dwStatus)
        printf("\n****** g_ITEAPI_TxSetChannelModulation error!!, %lu ******\n", dwStatus);
    else
        printf("\n****** g_ITEAPI_TxSetChannelModulation ok  ******\n\n");
    g_ChannelModulation_Setting=ChannelModulation_Setting;

    ChannelCapacity=tempBw*1000;
    ChannelCapacity=ChannelCapacity*(ChannelModulation_Setting.constellation*2+2);

    switch (ChannelModulation_Setting.interval) {
    case 0: //1/32
        ChannelCapacity=ChannelCapacity*32/33;
        break;
    case 1: //1/16
        ChannelCapacity=ChannelCapacity*16/17;
        break;
    case 2: //1/8
        ChannelCapacity=ChannelCapacity*8/9;
        break;
    case 3: //1/4
        ChannelCapacity=ChannelCapacity*4/5;
        break;
    }
    switch (ChannelModulation_Setting.highCodeRate) {
    case 0: //1/2
        ChannelCapacity=ChannelCapacity*1/2;
        break;
    case 1: //2/3
        ChannelCapacity=ChannelCapacity*2/3;
        break;
    case 2: //3/4
        ChannelCapacity=ChannelCapacity*3/4;
        break;
    case 3: //5/6
        ChannelCapacity=ChannelCapacity*5/6;
        break;
    case 4: //7/8
        ChannelCapacity=ChannelCapacity*7/8;
        break;
    }

    ChannelCapacity=ChannelCapacity/544*423;
    printf("The Maximum Channel Capacity is %lu bps(%lu Kbps)\n",ChannelCapacity,ChannelCapacity/1000);
    return(ChannelCapacity);
}

intmax_t GetFileSize(const char* filePath)
{
    struct stat statbuf;

    if(stat(filePath, &statbuf) == -1)
        return -1;
    return (intmax_t) statbuf.st_size;
}

uint32_t TxTransferTimeDelay(struct timeval start, struct timeval end)
{
    uint32_t diff, delay;

    diff = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    delay = gTransferInterval - diff;
    //printf("%d  ", delay);

    if (delay > 0)
        return delay;
    else
        return 0;
}


//A safe but slow way to calculate stream bit rate
Dword Get_DataBitRate(char *FilePath)
{
    int mPID = 0, i;
    const int BUFSIZE = 512*188; 
    Byte pbTSBuffData[BUFSIZE]; 
    Dword len,FileSize;
    Dword dwReadSize = 0;
    Dword FirstSyncBytePos=0;
    Dword FileOffset=0;
    ULONGLONG pcr1=0,pcr2=0;
    ULONGLONG pcr1_offset=0,pcr2_offset=0;
    Dword lStreamBitRate = 0;
    FILE *TsFile = NULL;
    intmax_t fileLength;
    unsigned PID;
    Dword pcrb,pcre;
    ULONGLONG pcr;	

    if(!(TsFile = fopen(FilePath, "r+"))) {
        printf("Open TS File Error!");
        return -1;
    }
    fileLength = GetFileSize(FilePath);

    len=FileSize=fileLength;
    if(len > BUFSIZE) len = BUFSIZE;

    fseek(TsFile, 0, SEEK_SET);	
    dwReadSize = fread(pbTSBuffData, 1, len, TsFile);

    //Find the first 0x47 sync byte
    for (FirstSyncBytePos=0;(FirstSyncBytePos<dwReadSize-188);FirstSyncBytePos++) 
    {
        if ((pbTSBuffData[FirstSyncBytePos] == 0x47)&&(pbTSBuffData[FirstSyncBytePos+188] == 0x47)) {
            //Sync byte found
            //printf("TS Sync byte found in offset: %lu\n",FirstSyncBytePos);
            break;
        }
    }

    if (FirstSyncBytePos>=dwReadSize-188) {
        printf("No sync byte found, it's not a valid 188-byte TS file!!!\n");
        return 0;
    }
    fseek(TsFile, FirstSyncBytePos, SEEK_SET);
    dwReadSize = fread(pbTSBuffData, 1, len, TsFile);

    FileOffset=FirstSyncBytePos;
    while (dwReadSize>188) 
    {		
        for (i = 0;(i<dwReadSize);i+=188) 
        {
            int adaptation_field_control ;

            adaptation_field_control = ((pbTSBuffData[i+3] >> 4) & 0x3);
            if (adaptation_field_control == 3 || adaptation_field_control == 2)
            {
                //CHECK adaptation_field_length !=0 && PCR flag on
                if (pbTSBuffData[i+4] != 0 && (pbTSBuffData[i+5] & 0x10) != 0) 	{
                    PID = 0;
                    pcrb = 0;
                    pcre = 0;
                    pcr = 0;

                    PID = ((pbTSBuffData[i+1] & 0x1f) << 8) |pbTSBuffData[i+2];	
                    pcrb=(pbTSBuffData[i+6]<<25) |
                      (pbTSBuffData[i+7]<<17) |
                      (pbTSBuffData[i+8]<<9) |
                      (pbTSBuffData[i+9]<<1) |
                      (pbTSBuffData[i+10]>>7) ;

                    pcre = ((pbTSBuffData[i+10] & 0x01) << 8) | (pbTSBuffData[i+11]);

                    pcr = (ULONGLONG)pcrb*300 + pcre;
                    //printf("Offset:%d,PID:%d(0x%x),PCRB:%u(0x%x),PCRE:%u(0x%x),PCR:%l(0x%lx)\n",FileOffset+i,PID,PID,pcrb,pcrb,pcre,pcre,pcr,pcr);
                    if (!pcr1)
                    {
                        pcr1=pcr;	
                        pcr1_offset=FileOffset+i;
                        mPID=PID;
                        printf("1'st PCR Offset: %lu\nPID: %d(0x%x), PCRB:%lu(0x%lx), PCRE:%lu(0x%lx), PCR:%llu(0x%llx)\n",FileOffset+i,PID,PID,pcrb,pcrb,pcre,pcre,pcr,pcr);
                        //printf("1'st PCR Offset: %lu\nPID: %u\nPCRB: %lu\nPCRE: %lu\nPCR: %llu\n",FileOffset+i,PID,pcrb,pcre,pcr);
                    }
                    else {
                        if (mPID==PID) 
                        {
                            pcr2=pcr;	
                            pcr2_offset=FileOffset+i;
                        }
                    }
                }
            }

        }
        FileOffset+=dwReadSize;

        //Both PCR1 & PCR2 are found,
        //If it's a large file (>20MB) , then we skip the middle part of the file,
        // and try to locate PCR2 in the last 10 MB in the file end
        // As PCR2 gets farther away from PCR1, the bit rate calculated gets more precise

        if (FileOffset>10000000 && FileOffset<FileSize-10000000 && pcr2) 
        {

            //Move to the last 10 MB position of file 
            fseek(TsFile, FileSize-10000000, SEEK_SET);				
            dwReadSize = fread(pbTSBuffData, 1, len, TsFile);				

            //Find the first 0x47 sync byte
            for (FirstSyncBytePos=0;(FirstSyncBytePos<dwReadSize-188);FirstSyncBytePos++) 
            {
                if ((pbTSBuffData[FirstSyncBytePos] == 0x47)&&(pbTSBuffData[FirstSyncBytePos+188] == 0x47))
                {
                    //Sync byte found
                    printf("TS Sync byte found in offset:%lu\n",FirstSyncBytePos+FileSize-10000000);
                    break;
                }
            }

            if (FirstSyncBytePos>=dwReadSize-188)
            {
                printf("No sync byte found in the end 10 MB of file!!!\n");
                break;
            }

            fseek(TsFile, FileSize-10000000+FirstSyncBytePos, SEEK_SET);				
            FileOffset=FileSize-10000000+FirstSyncBytePos;
        }

        dwReadSize = fread(pbTSBuffData, 1, len, TsFile);			
    }

    if (pcr2) {
        double fTmp =(double) ((pcr2_offset-pcr1_offset )* 8)/(double) ((pcr2 - pcr1));
        //float fTmp =(float) (pcr2_offset-pcr1_offset * 8) /(float) (pcr2 - pcr1);
        lStreamBitRate = (long)(fTmp*27000000);	
    }
    fclose(TsFile);
    return lStreamBitRate;
}


//Analyze PAT TSID and SID
long int Get_PAT_TSID_SID(FILE * TsFile,Byte *TSid,Byte *Sid,intmax_t FileLength)
{
    //int mPID = 0;
    unsigned i;
    unsigned int j;
    const int BUFSIZE = 512*188; 
    unsigned PID, psi_offset, sec_len,numofserv;	
    Byte pbTSBuffData[BUFSIZE]; 
    Dword   len,FileSize;

    len=FileSize=FileLength; 
    if(len > BUFSIZE) 		len = BUFSIZE;

    Dword dwReadSize = 0;
    Dword FirstSyncBytePos=0;
    Bool bPAT=False, bSDT=False,bNIT=False;

    fseek(TsFile, 0, SEEK_SET);	
    dwReadSize = fread(pbTSBuffData, 1, len, TsFile);

    //Find the first 0x47 sync byte
    for (FirstSyncBytePos=0;(FirstSyncBytePos<dwReadSize-188);FirstSyncBytePos++) 
    {
        if ((pbTSBuffData[FirstSyncBytePos] == 0x47)&&(pbTSBuffData[FirstSyncBytePos+188] == 0x47))
        {
            //Sync byte found
            //printf("TS Sync byte found in offset:%d\n",FirstSyncBytePos);
            break;
        }
    }

    if (FirstSyncBytePos>=dwReadSize-188) {
        printf("No sync byte found, it's not a valid 188-byte TS file!!!\n");

        return 0;
    }

    fseek(TsFile, FirstSyncBytePos, SEEK_SET);
    dwReadSize = fread(pbTSBuffData, 1, len, TsFile);

    while (dwReadSize>188&&!bPAT) 
    {		
        for (i=0;(i<dwReadSize);i+=188) 
        {
            PID=0; psi_offset=0; sec_len=0; numofserv=0;

            PID = ((pbTSBuffData[i+1] & 0x1f) << 8) |pbTSBuffData[i+2];	
            if (PID==0x11&&!bSDT)
            {
                bSDT=True;
                printf("Warning: SDT table already exists in this stream!\r\n");
            }

            if (PID==0&&!bPAT)
            {
                psi_offset=pbTSBuffData[i+4];

                if (pbTSBuffData[i+psi_offset+5]!=0) 
                {
                    //it's not PAT Table ID
                    continue;
                }

                sec_len=pbTSBuffData[i+psi_offset+7];
                memcpy(TSid, pbTSBuffData+i+psi_offset+8,2); //note it's big-endian
                numofserv=(sec_len-9)/4;
                for (j=0;j<numofserv;j++) 
                {
                    memcpy(Sid, pbTSBuffData+i+psi_offset+0xd+j*4,2); //note it's big-endian


                    if (Sid[0] || Sid[1]) 
                    {
                        //We only want the first TV service 
                        printf("PAT TS ID:0x%02x%02x and Service ID:0x%02x%02x found\r\n",TSid[0],TSid[1],Sid[0],Sid[1]);
                        bPAT=True;
                        break;
                    }
                    else 
                    {
                        //zero service id is NIT, instead of a service
                        bNIT=True;
                    }
                }

            }
        }
        if (bPAT&&bSDT) break;
        dwReadSize = fread(pbTSBuffData, 1, len, TsFile);			
    }
    if (!bPAT) printf("No PAT or Service ID found!\r\n");
    return 0;

}

void SetPeriodicCustomPacket(Handle TsFile, intmax_t FileLength, ModulatorParam param)
{
    //Test Periodical Custom Packets Insertion, (for SI/PSI table insertion)
    //Sample SDT
    Byte CustomPacket_1[188]={
        0x47,0x40,0x11,0x10,0x00,0x42,0xF0,0x36,0x00,0x99,0xC1,0x00,0x00,0xFF,0x1A,0xFF,
        0x03,0xE8,0xFC,0x80,0x25,0x48,0x23,0x01,0x10,0x05,0x49,0x54,0x45,0x20,0X20,0X20,
        0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X20,0x10,0x05,0x49,0x54,0x45,0x20,0x43,
        0x68,0x61,0x6E,0x6E,0x65,0x6C,0x20,0x31,0x20,0x20,0xFF,0xFF,0xFF,0xFF //LAST 4 BYTE=CRC32
    };

    //Sample NIT
    Byte CustomPacket_2[188]={
        0x47,0x40,0x10,0x10,0x00,0x40,0xf0,0x38,0xff,0xaf,0xc1,0x00,0x00,0xf0,0x0d,0x40,/*0x00000000*/
        0x0b,0x05,0x49,0x54,0x45,0x20,0x4f,0x66,0x66,0x69,0x63,0x65,0xf0,0x1e,0x00,0x99,/*0x00000010*/
        0xff,0x1a,0xf0,0x18,0x83,0x04,0x03,0xe8,0xfc,0x5f,0x5a,0x0b,0x02,0xeb,0xae,0x40,/*0x00000020*/
        0x1f,0x42,0x52,0xff,0xff,0xff,0xff,0x41,0x03,0x03,0xe8,0x01,0x1a,0xe6,0x2c,0x3f,/*0x00000030*/
    };

    Byte CustomPacket_3[188]={0x47,0x10,0x03,0x1c,0x00,0x00};
    Byte CustomPacket_4[188]={0x47,0x10,0x04,0x1c,0x00,0x00};
    Byte CustomPacket_5[188]={0x47,0x10,0x05,0x1c,0x00,0x00};

    int timer_interval, i, ret;
    Byte TSid[2]={0x00, 0x00}; //Note: it's big-endian
    Byte ONid[2]={0x01, 0x01};
    Byte NETid[2]={0x02, 0x02};
    Byte Sid[2]={0x00, 0x01};
    Byte ProviderName[16]={0x05,'i','T','E',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
    //In sample SDT table, the name lenther is fixed as 16 bytes
    //The first byte is Selection of character table, 0x5 is ISO/IEC 8859-9 [33] Latin alphabet No. 5
    Byte ServiceName[16]={0x05,'i','T','E',' ','C','H','1',0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
    //In sample SDT table, the name lenther is fixed as 16 bytes
    //The first byte is Selection of character table, 0x5 is ISO/IEC 8859-9 [33] Latin alphabet No. 5
    Byte NetworkName[11]={0x05,'i','T','E',' ','T','a','i','p','e','i'};
    //In sample NIT table, the name lenther is fixed as 11 bytes
    //The first byte is Selection of character table, 0x5 is ISO/IEC 8859-9 [33] Latin alphabet No. 5
    Byte LCN=0x50;   //Logical Channel Number

    Byte DeliveryParameters[3]={0x00, 0x00,0x00};
    unsigned  CRC_32 = 0;


    //GET TSID & SID from stream file
    Get_PAT_TSID_SID(TsFile,TSid,Sid,FileLength);

    /***************************************************************************************************/
    //	Custom SDT Table Insertion
    /***************************************************************************************************/
    //Set Sample SDT IN CustomPacket_1
    memcpy(CustomPacket_1+8 ,TSid,2);
    memcpy(CustomPacket_1+13,ONid,2);
    memcpy(CustomPacket_1+0x10,Sid,2);
    memcpy(CustomPacket_1+0x19,ProviderName,16);
    memcpy(CustomPacket_1+0x2A,ServiceName,16);

    CRC_32 = crc32(CustomPacket_1+5, 0x35);
    CustomPacket_1[0x3a] = ((CRC_32 >> 24) & 0xFF);
    CustomPacket_1[0x3b] = ((CRC_32 >> 16) & 0xFF);
    CustomPacket_1[0x3c] = ((CRC_32 >> 8) & 0xFF);
    CustomPacket_1[0x3d] = ((CRC_32) & 0xFF);


    // Set & Copy SDT packets to internal buffer 1

    if (g_ITEAPI_TxSetPeridicCustomPacket(188, CustomPacket_1, 1))
        printf("g_ITEAPI_TxAccessFwPSITable 1 fail\n");

    //printf("Enter the timer interval for custom table packet buffer 1, SDT (in ms, 0 means disabled): ");
    //scanf("%d", &timer_interval);

    timer_interval=20; //Set SDT repetition rate to 20 ms, 

    if (g_ITEAPI_TxSetPeridicCustomPacketTimer(1, timer_interval))		
        printf("g_ITEAPI_TxSetFwPSITableTimer  %d fail\n",1);

    return ;

    //The following codes are for programmer's reference to customize other SI tables in table packet buffer 2~5

    /***************************************************************************************************/
    //	Custom NIT Table Insertion (set LCN and Delivery descriptor)
    /***************************************************************************************************/

    //Set Sample NIT IN CustomPacket_2
    memcpy(CustomPacket_2+8   ,NETid,2);
    memcpy(CustomPacket_2+0x11,NetworkName,11);
    memcpy(CustomPacket_2+0x1E,TSid,2);
    memcpy(CustomPacket_2+0x20,ONid,2);


    //Set LCN Descriptor:0x83
    memcpy(CustomPacket_2+0x26,Sid,2);
    memcpy(CustomPacket_2+0x29,&LCN,1);


    //Set Delievery Descriptor:0x5a, NOTE:it's in 10Hz, instead of KHz or Hz
    CustomPacket_2[0x2c] = (Byte)((param.Frequency*100 >> 24) & 0xFF);
    CustomPacket_2[0x2d] = (Byte)((param.Frequency*100 >> 16) & 0xFF);
    CustomPacket_2[0x2e] = (Byte)((param.Frequency*100 >> 8) & 0xFF);
    CustomPacket_2[0x2f] = (Byte)((param.Frequency*100) & 0xFF);


    //Bandwidth 000:8M, 001:7,010:6,011:5
    DeliveryParameters[0]|=(abs(param.Bandwidth/1000-8)<<5);
    //Priority,timeslice, MPEFEC, reserved bits
    DeliveryParameters[0]|=0x1f;



    //Constellation:0: QPSK, 1: 16QAM, 2: 64QAM
    DeliveryParameters[1]|=(g_ChannelModulation_Setting.constellation<<6);
    //CR:0: 1/2, 1: 2/3, 2: 3/4, 3:5/6, 4: 7/8
    DeliveryParameters[1]|=(g_ChannelModulation_Setting.highCodeRate);


    //GI:0: 1/32, 1: 1/16, 2: 1/8, 3: 1/4
    DeliveryParameters[2]|=(g_ChannelModulation_Setting.interval<<3);

    //Transmission mode: 0: 2K, 1: 8K  2:4K
    DeliveryParameters[2]|=(g_ChannelModulation_Setting.transmissionMode<<1);

    memcpy(CustomPacket_2+0x30,DeliveryParameters,3);


    //Set Service list descriptror:0x41
    memcpy(CustomPacket_2+0x39,Sid,2);


    CRC_32 = crc32(CustomPacket_1+5, 0x37);
    CustomPacket_2[0x3c] = ((CRC_32 >> 24) & 0xFF);
    CustomPacket_2[0x3d] = ((CRC_32 >> 16) & 0xFF);
    CustomPacket_2[0x3e] = ((CRC_32 >> 8) & 0xFF);
    CustomPacket_2[0x3f] = ((CRC_32) & 0xFF);



    if (g_ITEAPI_TxSetPeridicCustomPacket(188, CustomPacket_2, 2))
        printf("g_ITEAPI_TxAccessFwPSITable 2 fail\n");


    printf("Enter the timer interval for custom packet table 2, NIT (in ms, 0 means disabled): ");
    ret = scanf("%d", &timer_interval);


    if (g_ITEAPI_TxSetPeridicCustomPacketTimer(2, timer_interval) )		
        printf("g_ITEAPI_TxSetFwPSITableTimer  %d fail\n",2);



    /***************************************************************************************************/
    //	Other Custom Table Insertion
    /***************************************************************************************************/


    if (g_ITEAPI_TxSetPeridicCustomPacket(188, CustomPacket_3, 3) == 0)
        printf("g_ITEAPI_TxAccessFwPSITable 3 ok\n");
    else
        printf("g_ITEAPI_TxAccessFwPSITable 3 fail\n");
    if (g_ITEAPI_TxSetPeridicCustomPacket(188, CustomPacket_4, 4) == 0)
        printf("g_ITEAPI_TxAccessFwPSITable 4 ok\n");
    else
        printf("g_ITEAPI_TxAccessFwPSITable 4 fail\n");
    if (g_ITEAPI_TxSetPeridicCustomPacket(188, CustomPacket_5, 5) == 0)
        printf("g_ITEAPI_TxAccessFwPSITable 5 ok\n");
    else
        printf("g_ITEAPI_TxAccessFwPSITable 5 fail\n");

    for (i=3;i<=5;i++) {
        printf("Enter the timer interval for custom packet %d (in ms, 0 means disabled): ", i);
        ret = scanf("%d", &timer_interval);
        if (g_ITEAPI_TxSetPeridicCustomPacketTimer(i, timer_interval))		
            printf("g_ITEAPI_TxSetFwPSITableTimer  %d fail\n",i);
    }
}

unsigned long GetTickCount()
{
    //struct tms tm;
    //return (times(&tm)*1000)/sysconf(_SC_CLK_TCK); // Return ticks. One tick = 1/1000 seconds.

    //struct timespec ts;	
    //gettimeofday(&ts, NULL);
    //printf("%d\n", ts.tv_sec*1000 + ts.tv_nsec/1000);
    //return (ts.tv_sec*1000 + ts.tv_nsec/1000);


    struct timeval tv;
    unsigned long utime;	
    gettimeofday(&tv,NULL);	
    utime = tv.tv_sec * 1000 + tv.tv_usec/1000;	
    //printf("utime: [%u][%d]\n", tv.tv_sec, tv.tv_usec);
    return utime;

    //clock_t tv;
    //printf("%d\n", clock());
    //return clock(); 
}

void mydelay(Dword delay)
{
    clock_t goal = delay * (CLOCKS_PER_SEC /1000) + clock();
    while(goal > clock());
}

void TX_DataOutputTest(ModulatorParam param)
{
    int input = 0;
    Handle TsFile = NULL;
    intmax_t dwFileSize = 0;
    char FilePath[128] = "output.ts";
    Byte *fileBuf = NULL;
    Dword Tx_datalength = TRANSMITTER_BLOCK_SIZE;
    int i = 0, k = 0;

    Dword LoopStartTime = 0;
    ULONGLONG FirstSyncBytePos=0;
    ULONGLONG bytesRead,Total_bytesSent;
    Bool Play_InfiniteLoop = False;
    ULONGLONG TSFileDataRate;
    int LoopCnt=0;

    Dword Txbytes_PerMs = 0;
    Dword DelayTime_PerTx = 0;	

    Dword TxStartTime = 0;
    Dword PCRAdaptationOffset = 0;
    char key;
    Dword ret;
    ULONGLONG Output_Data_Rate;

    struct timespec req = {0};
    req.tv_sec = 0;     
    req.tv_nsec = 1;    

    printf("Input the TS file path name:  ");
    ret = scanf("%s", FilePath); // buffer size is 10, width specification is 9

    if(!(TsFile = fopen(FilePath, "r+"))) {
        printf("Open TS File Error!");
        return;
    }

    if (TsFile == INVALID_HANDLE_VALUE)	{
        printf("Open Ts File Error!!!\n");
        return ;
    }

    dwFileSize = GetFileSize(FilePath);
    printf("%s size = %9jd\n", FilePath, dwFileSize);

    printf("Insert Periodical Custom Packets for SI SDT Table (y:yes) ?");	 
    ret = scanf("\n%c", &key);

    printf("\n");
    if (key=='y'||key=='Y')
    {
        SetPeriodicCustomPacket(TsFile, dwFileSize, param);
    }
    else
    {
        //Disable Custom table packet insertion
        for (i=1;i<=5;i++) {
            if (g_ITEAPI_TxSetPeridicCustomPacketTimer(i, 0))		
                printf("g_ITEAPI_TxSetFwPSITableTimer  %d fail\n",i);
        }
    }
    //Check input file data bit rate here, slow but safe
    TSFileDataRate=Get_DataBitRate(FilePath);
    printf("The recommended input file data rate for %s is = %llu KBps\n",FilePath ,TSFileDataRate/1000);

    printf("Enter Data Bit rate in Kbps(ex. 10000 for 10 Mbps):  ");
    ret = scanf("%d", &input);

    TSFileDataRate=input*1000;  //convert to bps
    Txbytes_PerMs = (input * 1000 / 8) / 1000; // "input / 8 * 1000000" will increase inaccuracy	

    if(Txbytes_PerMs == 0)
        DelayTime_PerTx = 0;
    else
        DelayTime_PerTx = Tx_datalength / Txbytes_PerMs;	

    if (TSFileDataRate>g_ChannelCapacity)
        printf("Warning the input file data rate (%llu bps) is larger than channel data rate(%lu bps)",TSFileDataRate,g_ChannelCapacity);

    printf("Repeat Loop: 1.Repeat, 2.Once:  ");
    ret = scanf("%d", &input);
    if(input == 1)
        Play_InfiniteLoop = True;

    printf("Press 'A' or 'a' to abort...\n");
    fileBuf = (Byte *)malloc(Tx_datalength);

    //Find the first 0x47 sync byte
    fseek(TsFile, 0, SEEK_SET);	
    bytesRead = fread(fileBuf, 1, Tx_datalength, TsFile);

    for (FirstSyncBytePos=0;(FirstSyncBytePos<bytesRead-188);FirstSyncBytePos++) 
    {
        if ((fileBuf[FirstSyncBytePos] == 0x47)&&(fileBuf[FirstSyncBytePos+188] == 0x47))
        {
            //Sync byte found
            printf("TS Sync byte found in offset:%llu\n",FirstSyncBytePos);
            break;
        }
    }

    if (FirstSyncBytePos>=bytesRead-188) {
        printf("No sync byte found, it's not a valid 188-byte TS file!!!\n");
        goto TX_DataOutputTest_exit;
    }

    Total_bytesSent=0;
    LoopCnt=0;

    fseek(TsFile, FirstSyncBytePos, SEEK_SET);	
    LoopStartTime =TxStartTime= GetTickCount();
    printf("LoopStartTime: %lu\n", LoopStartTime);
    printf("Loop Repeat: %d\n", LoopCnt);	
    g_ITEAPI_StartTransfer();

    memset(fileBuf, 0, Tx_datalength);
    bytesRead = fread(fileBuf, 1, Tx_datalength, TsFile);	
    ret = g_ITEAPI_TxSendTSData((Byte*)fileBuf, bytesRead);	

    TxStartTime = GetTickCount();
    //start to control bit rate	
    //usleep(DelayTime_PerTx*1000);

    while(1){
        //memset(fileBuf, 0, Tx_datalength);
        bytesRead = fread(fileBuf, 1, Tx_datalength, TsFile);

rewrite_case1:
        ret = g_ITEAPI_TxSendTSData((Byte*)fileBuf, bytesRead);

        if (ret == 0 && (bytesRead >= Tx_datalength)) {
            usleep(100);
            printf("rewrite\n");
            goto rewrite_case1;		
        } else if(ret == 0 || ret == ERR_NOT_IMPLEMENTED) {
            usleep(100000);
            printf("write fail\n");
            k++;
            if (k>50) {
                break; // 5sec
            }
            goto rewrite_case1;	
        }	
        Total_bytesSent+=bytesRead;

        //Is it the end of file ?
        if(bytesRead < Tx_datalength){
            printf("End of file reached..Total loop time: %lu ms\n", GetTickCount()-LoopStartTime);
            if(Play_InfiniteLoop)
            {
                //Repeat Loop 
                printf("Press 'A' or 'a' to abort...\n");
                g_ITEAPI_StopTransfer();	
                usleep(3000000);			
                g_ITEAPI_StartTransfer();				
                fseek(TsFile, FirstSyncBytePos, SEEK_SET);	
                LoopStartTime= GetTickCount();
                PCRAdaptationOffset=(LoopStartTime-TxStartTime) * 90 / 2; //Change system time offset(in ms) to PCR's tick count offset (in 45KHz)
                Total_bytesSent=0;
                LoopCnt++;
                printf("Loop Repeat: %d\n", LoopCnt);
            }
            else
                break;
        }

        //Data Rate Control, IF the TS data rate is less than channel modulation data rate
        //and excluding the first time check (divide by zero check) 
        while ((TSFileDataRate<g_ChannelCapacity-1024)&&Total_bytesSent)
        {
            Output_Data_Rate=0;
            if (GetTickCount()==LoopStartTime) continue;

            Output_Data_Rate= Total_bytesSent*8/(GetTickCount()-LoopStartTime)*1000;			
            if (Output_Data_Rate<=TSFileDataRate)
            {
                break;
            }
            else nanosleep(&req, &req);
        }

        if ( kbhit() !=0 ){
            key=(char)getchar();
            if (key=='a' || key=='A') {
                Play_InfiniteLoop = False;
                break;
            }
        } 

    }

TX_DataOutputTest_exit:
    g_ITEAPI_StopTransfer();
    if(fileBuf) free (fileBuf);
    if(TsFile) fclose(TsFile);
}
/*
static int TxOutOnOff()
{
    int cho, ret;
    Dword dwStatus = 0;

    printf("\n=> Please Input Tx mode ON or OFF (0:Off  1:On): ");
    ret = scanf("%d", &cho);

    if (cho == 1)
        dwStatus = g_ITEAPI_SetTxMode(1);
    else if (cho == 0)
        dwStatus = g_ITEAPI_SetTxMode(0);
    else {
        dwStatus = 1;
        printf("\nInvaild value !!, %d", cho);
    }

    if (dwStatus)
        printf("\nSetTxMode Error!!, %lu", dwStatus);
    else
        printf("\n******  SetTxMode %s DONE  *******\n\n", cho?"On":"Off");

    return 0;
}
*/
int TxAutoChangeModule()
{
    int i, k = 0;
    int ct, it, cr;
    Dword dwStatus = 0;
    long fileLength = 0;
    char *fileBuf = NULL;
    FILE *fp = NULL;
    int writeLen = TX_DATA_LEN;
    struct timeval start, end;
    uint32_t diff;
    int dwRead = 0;
    int repeatCount;
    int ret = 0;
    int txStart = 0;
    MODULATION_PARAM ChannelModulation_Setting;	

    //for (tm = 1; tm >= 0; tm--) {
    for(ct = 0; ct < 3; ct++) {
        for (it = 0; it < 4; it++) {
            for (cr = 0; cr < 5; cr++) {
                //printf("\ntransmissionMode: %d, constellation: %d, interval: %d, highCodeRate: %d\n", 1, ct, it, cr);
                ChannelModulation_Setting.transmissionMode = 1;
                ChannelModulation_Setting.constellation = ct;
                ChannelModulation_Setting.interval = it;
                ChannelModulation_Setting.highCodeRate = cr;
                dwStatus = g_ITEAPI_TxSetChannelModulation(ChannelModulation_Setting);
                if (dwStatus)
                    printf("\nSetModule Error!!, %lu", dwStatus);
                else
                    printf("\n******  SetModule DONE  ******\n\n");

                if (txStart == 0) {
                    fileLength = GetFileSize("/home/encode_Produce.ts");	
                    fileBuf = (char *)malloc(writeLen); 
                    if (!(fp = fopen("/home/encode_Produce.ts", "r+"))) {
                        printf("File open fail!");
                        return -1;
                    }
                    printf("File Length is %ld, Tx Length is %d\n", fileLength, writeLen);
                    g_ITEAPI_StartTransfer();
                    txStart = 1;
                }

                repeatCount = 3;
                while (repeatCount) {
                    for (i = 0; i < (fileLength / writeLen); i++) {
                        gettimeofday(&start, NULL);
                        dwRead = fread(fileBuf, 1, writeLen, fp);
rewrite:
                        ret = g_ITEAPI_TxSendTSData((Byte*)fileBuf, writeLen);
                        if (ret == 0 && i != ((fileLength / writeLen) -1)) {
                            usleep(50);
                            //printf("rewrite\n");			
                            goto rewrite;		
                        } else if (ret < 0 || ret == ERR_NOT_IMPLEMENTED) {
                            usleep(100000);
                            printf("write fail\n");			
                            k++;
                            if (k>50) {
                                break; // 5sec
                            }
                            goto rewrite;	
                        }

                        gettimeofday(&end, NULL);	
                        diff = TxTransferTimeDelay(start, end);
                        if (diff > 0 && diff <= gTransferInterval && gRateControl)
                            usleep(diff * 1000);

                    }

                    fseek(fp, 0, SEEK_SET);
                    repeatCount--;
                    printf("Remaining Repeat times = %d\n",repeatCount);
                }	
            }	
        }
    }

    fclose(fp);
    free(fileBuf);

    g_ITEAPI_StopTransfer();
    txStart = 0;
    //}
    return 0;
}

static void echoTX(ModulatorParam param)
{
    //Disable Custom table packet insertion
    for (int i = 1; i <= 5; i++) {
        if (g_ITEAPI_TxSetPeridicCustomPacketTimer(i, 0))		
            printf("g_ITEAPI_TxSetFwPSITableTimer  %d fail\n",i);
    }

    g_ITEAPI_StartTransfer();

    int nFailCount = 0;
    Byte packet[188]={0x47,0x1f,0xaf,0x1c,'h','e','l','l','o','\n','\0'};
    while (true) {
        Dword nPktSize = 188;
        Dword ret = g_ITEAPI_TxSendTSData((Byte*) packet, nPktSize);
        if (ret == 0 || ret == ERR_NOT_IMPLEMENTED) {
            if (++nFailCount > 50) // 5 sec
                break;
            printf("write fail\n");
            usleep(100*1000);
        } else {
            nFailCount = 0;//reset
        }

        usleep(1000);
    }

    g_ITEAPI_StopTransfer();
}

static void ule_tx(ModulatorParam param)
{
    //Disable Custom table packet insertion
    for (int i = 1; i <= 5; i++) {
        if (g_ITEAPI_TxSetPeridicCustomPacketTimer(i, 0))		
            printf("g_ITEAPI_TxSetFwPSITableTimer  %d fail\n",i);
    }

    g_ITEAPI_StartTransfer();
    int temp = 0;
    printf("\n=> Please Input Send count: ");
    scanf("%d", &temp);
    
    int nTotalCount = 0;
    for (int i = 0; i < temp; i++) {
        int nSize = 200;
        unsigned char data[nSize];
        memset(data, '0', nSize);
        
        SNDUInfo info;
        ule_init(&info, IPv4, data, nSize);
        uint32_t totalLength = ule_getTotalLength(&info);
        unsigned char pkt[totalLength];
        ule_encode(&info, pkt, totalLength);

        ULEEncapCtx ctx;
        ule_initEncapCtx(&ctx);
        ctx.pid = 0x1FAF;
        ctx.snduPkt = pkt;
        ctx.snduLen = totalLength;

        int count = 0;
        int nFailCount = 0;
        while (ctx.snduIndex < ctx.snduLen) {
            ule_padding(&ctx);
            //hexdump(ctx.tsPkt, 188);
            Dword nPktSize = 188;
            
            Dword ret = g_ITEAPI_TxSendTSData((Byte*) ctx.tsPkt, nPktSize);
            
            //Byte packet[188]={0x47,0x1f,0xaf,0x1c,'h','e','l','l','o','\n','\0'};
            //Dword ret = g_ITEAPI_TxSendTSData((Byte*) packet, nPktSize);
            
            printf("send pktSize=%lu\n", nPktSize);
            if (ret == 0 || ret == ERR_NOT_IMPLEMENTED) {
                if (++nFailCount > 50)
                    break;
                printf("write fail!\n");
                usleep(100*1000);
            }
            else {
                nFailCount = 0;// reset
            }
            count++;
            usleep(100*1000);
        }
        nTotalCount++;
        printf("send: size=%d, count=%d, total=%d\n", nSize, count, nTotalCount);
    }
    

    g_ITEAPI_StopTransfer();
}

//int main(int argc, char **argv)
int tx(Byte handleNum)
{
    int chose, adjustoutputgain, outgainvalue;
    int closeFlag = 0, ret;
    int device_type = 0;
    Dword tempValue = 0;
    Dword frequency = 0;
    Dword bandwidth = 0;
    int MaxGain = 0;
    int MinGain = 0;	
    int Gain = 0;
    //Byte handleNum;
    ModulatorParam param;
    TPS tps;

    memset(&param, 0, sizeof(param));

    if(GetDriverInfo(handleNum) != ERR_NO_ERROR)
        return 0;

    while (!closeFlag) {
        printf("\n\n========= ITEtech Linux IT950x API TestKit =========");
        printf("\n1. Set Modulation Transmission Parameters             ");
        printf("\n2. Set Device/Board Type                              ");
        printf("\n3. Set RF output Gain/Attenuation                     ");
        printf("\n4. Transmission Parameter Signalling Cell-id Setting  ");	
        printf("\n5. Output Test (Streaming a TS File)                  ");
        printf("\n6. Send Hello msg                                     ");
        printf("\n7. ULE test                                           ");
        printf("\n0. Quit                                               ");
        printf("\nEnter Number: ");		
        ret = scanf("%d", &chose);

        switch (chose) {
        case 1:
            g_ChannelCapacity = TX_SetChannelTransmissionParameters(&param);
            break;
        case 2:
            g_ITEAPI_GetDeviceType((Byte*)&device_type);
            printf("\nCurrent Device Type Setting: %d\n", device_type);
            printf("Enter Device Type (0: EVB, 1:DB-01-01 v01, 2:DB-01-02 v01, 3:DB-01-01 v03):  ");
            ret = scanf("%d", &device_type);
            if(device_type < 0 || device_type > 3) device_type = 0;
            if (g_ITEAPI_SetDeviceType((Byte)device_type) == ERR_NO_ERROR)
                printf("g_ITEAPI_SetDeviceType ok\n");
            else
                printf("g_ITEAPI_SetDeviceType error\n");					
            break;
        case 3:
            Gain = 0;	
            MaxGain = 0;
            MinGain = 0;			
            frequency = param.Frequency;
            bandwidth = param.Bandwidth;
            printf("\nFrequency: %lu, ", frequency);
            printf("Bandwidth: %lu, ", bandwidth);
            if(g_ITEAPI_GetGainRange(frequency, (Word)bandwidth, &MaxGain, &MinGain) != ERR_NO_ERROR)
                break;
            printf("MinGain: %d, MaxGain: %d\n", MinGain, MaxGain);				

            if(g_ITEAPI_TxGetOutputGain(&Gain) != 0) {
                printf("g_ITE_TxGetOutputGain error\n");
                break;
            }
            printf("=> Please Input Gain/Attenuation (Current Setting: %d): ", Gain);
            ret = scanf("%d", &adjustoutputgain);
            if(g_ITEAPI_AdjustOutputGain(adjustoutputgain, &outgainvalue) == ERR_NO_ERROR)
                printf("g_ITEAPI_AdjustOutputGain ok: %d dB\n", outgainvalue);
            else
                printf("g_ITEAPI_AdjustOutputGain error\n");					
            break;
        case 4: {
            memset(&tps, 0, sizeof(tps));
            tempValue = 0;

            //Get current TPS setting
            if(g_ITEAPI_TxGetTPS(&tps) == 0) {
                printf("=====================\n");
                printf("Current TPS:\n");
                printf("cellid = %d\n", tps.cellid);
                /*
                   printf("constellation = %d\n", tps.constellation);
                   printf("highCodeRate = %d\n", tps.highCodeRate);
                   printf("interval = %d\n", tps.interval);
                   printf("lowCodeRate = %d\n", tps.lowCodeRate);
                   printf("transmissionMode = %d\n", tps.transmissionMode);
                 */
                printf("=====================\n");
            }
            else
                printf("g_ITEAPI_TxGetTPS error\n");

            printf("Set TPS cellid = ");
            ret = scanf("%lu", &tempValue);
            tps.cellid = tempValue;
            /*
               printf("constellation = ");
               ret = scanf("%u", &tempValue);					
               tps.constellation = (BYTE)tempValue;
               printf("highCodeRate = ");
               ret = scanf("%u", &tempValue);
               tps.highCodeRate = (BYTE)tempValue;
               printf("interval = ");
               ret = scanf("%u", &tempValue);
               tps.interval = (BYTE)tempValue;
               printf("lowCodeRate = ");
               ret = scanf("%u", &tempValue);
               tps.lowCodeRate = (BYTE)tempValue;
               printf("transmissionMode = ");
               ret = scanf("%u", &tempValue);
               tps.transmissionMode = (BYTE)tempValue;
             */
            if(g_ITEAPI_TxSetTPS(tps) == 0) {
                printf("g_ITEAPI_TxSetTPS ok\n");						
            }
            else
                printf("g_ITEAPI_TxSetTPS error\n");

            break;
        }	
        case 5:
            gRateControl = 1;
            TX_DataOutputTest(param);				
            break;
        case 6:
            echoTX(param);
            break;
        case 7:
            ule_tx(param);
            break;
        case 0:
            closeFlag = 1;
        }
    }// end of while
    g_ITEAPI_Finalize();

    return 0;
}
