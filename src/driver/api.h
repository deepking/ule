//
// Copyright (c) 2012 ITE Technologies, Inc. All rights reserved.
// 
// Date:
//    2012/09/18
//
// Module Name:
//    api.h
//
//Abstract:
//    ITE Linux API header file.
//


#ifndef     _DEMOD_DTVAPI_
#define     _DEMOD_DTVAPI_

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <termios.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>

#include "type.h"
#include "iocontrol.h"
#include "error.h"


#define DTVEXPORT
#define __DTVAPI_VERSION__      "1.0.0.0"
#define INVALID_HANDLE_VALUE    0
#define TRANSMITTER_BLOCK_SIZE 188*174
//extern int g_hDriver = 0;
//
// The type defination of StreamType.
//
typedef enum {
	DTVStreamType_NONE = 0,		// Invalid (Null) StreamType
	DTVStreamType_DVBH_DATAGRAM,
	DTVStreamType_DVBH_DATABURST,
	DTVStreamType_DVBT_DATAGRAM,
	DTVStreamType_DVBT_PARALLEL,
	DTVStreamType_DVBT_SERIAL,
	DTVStreamType_TDMB_DATAGRAM,
	DTVStreamType_FM_DATAGRAM,
	DTVStreamType_FM_I2S
} DTVStreamType;

//
// Driver and API information.
//
typedef struct {
    Byte  DriverVerion[16];      // XX.XX.XX.XX Ex., 1.2.3.4.
    Byte  APIVerion[32];         // XX.XX.XXXXXXXX.XX Ex., 1.2.3.4.
    Byte  FWVerionLink[16];      // XX.XX.XX.XX Ex., 1.2.3.4.
    Byte  FWVerionOFDM[16];      // XX.XX.XX.XX Ex., 1.2.3.4.
    Byte  DateTime[24];          // Ex.,"2004-12-20 18:30:00" or "DEC 20 2004 10:22:10" with compiler __DATE__ and __TIME__  definitions.
    Byte  Company[8];            // Ex.,"Afatech".
    Byte  SupportHWInfo[32];     // Ex.,"Jupiter DVBT/DVBH".
    Dword error;
    Byte  reserved[128];
} DTVDriverInfo, *PDTVDriverInfo;

//
// The type defination of Priority.
//
typedef enum {
    DTVPriority_HIGH = 0,        // DVB-T and DVB-H - identifies high-priority stream.
    DTVPriority_LOW              // DVB-T and DVB-H - identifies low-priority stream.
} DTVPriority;

//
// The type defination of IpVersion.
//
typedef enum {
	DTVIpVersion_IPV4 = 0,       // The IP version if IPv4.
	DTVIpVersion_IPV6 = 1        // The IP version if IPv6.
} DTVIpVersion;


//
// The type defination of Ip.
//
typedef struct {
	DTVIpVersion version;        // The version of IP. See the defination of IpVersion.
	DTVPriority priority;        // The priority of IP. See the defination of Priority.
	Bool cache;                  // True: IP datagram will be cached in device's buffer. Fasle: IP datagram will be transfer to host.
	Byte address[16];            // The byte array to store IP address.
} DTVIp, *PDTVIp;

//
// The type defination of Platform.
// Mostly used is in DVB-H standard
//
typedef struct {
	Dword platformId;            // The ID of platform.
	char iso639LanguageCode[3];  // The ISO 639 language code for platform name.
	Byte platformNameLength;     // The length of platform name.
	char platformName[32];       // The char array to store platform name.
	Word bandwidth;              // The operating channel bandwith of this platform.
	Dword frequency;             // The operating channel frequency of this platform.
	Byte information[4096];      // The extra information about this platform.
	Word informationLength;      // The length of information.
	Bool hasInformation;         // The flag to indicate if there exist extra information.
	DTVIpVersion ipVersion;      // The IP version of this platform.
} DTVPlatform, *PDTVPlatform;

//
// The type defination of Target.
//
typedef enum {
	DTVSectionType_MPE = 0,      // Stands for MPE data.
	DTVSectionType_SIPSI,        // Stands for SI/PSI table, but don't have to specify table ID.
	DTVSectionType_TABLE         // Stands for SI/PSI table.
} DTVSectionType;

//
// The type defination of FrameRow.
//
typedef enum {
	DTVFrameRow_256 = 0,         // There should be 256 rows for each column in MPE-FEC frame.
	DTVFrameRow_512,             // There should be 512 rows for each column in MPE-FEC frame.
	DTVFrameRow_768,	           // There should be 768 rows for each column in MPE-FEC frame.
	DTVFrameRow_1024             // There should be 1024 rows for each column in MPE-FEC frame.
} DTVFrameRow;

//
// The type defination of Pid.
//
// as sectionType = SectionType_SIPSI: only value is valid.
// as sectionType = SectionType_TABLE: both value and table is valid.
// as sectionType = SectionType_MPE: except table all other fields is valid.
//
typedef struct {
	Byte table;	                 // The table ID. Which is used to filter specific SI/PSI table.
	Byte duration;               // The maximum burst duration. It can be specify to 0xFF if user don't know the exact value.
	DTVFrameRow frameRow;        // The frame row of MPE-FEC. It means the exact number of rows for each column in MPE-FEC frame.
	DTVSectionType sectionType;  // The section type of pid. See the defination of SectionType.
	DTVPriority priority;        // The priority of MPE data. Only valid when sectionType is set to SectionType_MPE.
	DTVIpVersion version;        // The IP version of MPE data. Only valid when sectionType is set to SectionType_MPE.
	Bool cache;                  // True: MPE data will be cached in device's buffer. Fasle: MPE will be transfer to host.
	Word value;                  // The 13 bits Packet ID.
} DTVPid, *PDTVPid;

//
// The type defination of statistic
//
typedef struct {
    Dword postVitErrorCount;    // ErrorBitCount.
    Dword postVitBitCount;      // TotalBitCount.
    Word abortCount;            // Number of abort RSD packet.
    Word signalQuality;         // Signal quality (0 - 100).
    Word signalStrength;        // Signal strength (0 - 100).
    Bool signalPresented;       // TPS lock.
    Bool signalLocked;          // MPEG lock.
    Byte frameErrorCount;       // Frame Error Ratio (error ratio before MPE-FEC) = frameErrorRate / 128.
    Byte mpefecFrameErrorCount; // MPE-FEC Frame Error Ratio (error ratio after MPE-FEC) = mpefecFrameErrorCount / 128.
} DTVStatistic, *PDTVStatistic;

//
// The type defination of TransmissionMode.
//
typedef enum {
    DTVTransmissionMode_2K = 0,  // OFDM frame consists of 2048 different carriers (2K FFT mode).
    DTVTransmissionMode_8K = 1,  // OFDM frame consists of 8192 different carriers (8K FFT mode).
    DTVTransmissionMode_4K = 2   // OFDM frame consists of 4096 different carriers (4K FFT mode).
} DTVTransmissionMode;

//
// The type defination of Constellation.
//
typedef enum {
    DTVConstellation_QPSK = 0,   // Signal uses QPSK constellation.
    DTVConstellation_16QAM,      // Signal uses 16QAM constellation.
    DTVConstellation_64QAM       // Signal uses 64QAM constellation.
} DTVConstellation;

//
// The type defination of Interval.
//
typedef enum {
    DTVInterval_1_OVER_32 = 0,   // Guard interval is 1/32 of symbol length.
    DTVInterval_1_OVER_16,       // Guard interval is 1/16 of symbol length.
    DTVInterval_1_OVER_8,        // Guard interval is 1/8 of symbol length.
    DTVInterval_1_OVER_4         // Guard interval is 1/4 of symbol length.
} DTVInterval;

//
// The type defination of CodeRate.
///
typedef enum {
    DTVCodeRate_1_OVER_2 = 0,    // Signal uses FEC coding ratio of 1/2.
    DTVCodeRate_2_OVER_3,        // Signal uses FEC coding ratio of 2/3.
    DTVCodeRate_3_OVER_4,        // Signal uses FEC coding ratio of 3/4.
    DTVCodeRate_5_OVER_6,        // Signal uses FEC coding ratio of 5/6.
    DTVCodeRate_7_OVER_8,        // Signal uses FEC coding ratio of 7/8.
    DTVCodeRate_NONE             // None, NXT doesn't have this one.
} DTVCodeRate;

//
// TPS Hierarchy and Alpha value.
//
typedef enum {
    DTVHierarchy_NONE = 0,       // Signal is non-hierarchical.
    DTVHierarchy_ALPHA_1,        // Signalling format uses alpha of 1.
    DTVHierarchy_ALPHA_2,        // Signalling format uses alpha of 2.
    DTVHierarchy_ALPHA_4         // Signalling format uses alpha of 4.
} DTVHierarchy;

//
// The type defination of Bandwidth.
//
typedef enum {
    DTVBandwidth_6M = 0,         // Signal bandwidth is 6MHz.
    DTVBandwidth_7M,             // Signal bandwidth is 7MHz.
    DTVBandwidth_8M,             // Signal bandwidth is 8MHz.
    DTVBandwidth_5M              // Signal bandwidth is 5MHz.
} DTVBandwidth;

//
// The defination of ChannelInformation.
//
typedef struct {
    Dword frequency;                         // Channel frequency in KHz.
    DTVTransmissionMode transmissionMode;    // Number of carriers used for OFDM signal.
    DTVConstellation constellation;          // Constellation scheme (FFT mode) in use.
    DTVInterval interval;                    // Fraction of symbol length used as guard (Guard Interval).
    DTVPriority priority;                    // The priority of stream.
    DTVCodeRate highCodeRate;                // FEC coding ratio of high-priority stream.
    DTVCodeRate lowCodeRate;                 // FEC coding ratio of low-priority stream.
    DTVHierarchy hierarchy;                  // Hierarchy levels of OFDM signal.
    DTVBandwidth bandwidth;
} DTVChannelTPSInfo, *PDTVChannelTPSInfo;

//
// Temp data structure of Gemini.
//
typedef struct
{
    Byte charSet;
    Word charFlag;
    Byte string[16];
} DTVLabel, *PDTVLabel;

//
// Temp data structure of Gemini.
//
typedef struct
{
    Word ensembleId;
    DTVLabel ensembleLabel;
    Byte totalServices;
} DTVEnsemble, *PDTVEnsemble;

//
// The type defination of Service.
// Mostly used is in T-DMB standard
//
typedef struct {
    Byte serviceType;            // Service Type(P/D): 0x00: Program, 0x80: Data.
    Dword serviceId;
    Dword frequency;
    DTVLabel serviceLabel;
    Byte totalComponents;
} DTVService, *PDTVService;

//
// The type defination of Service Component.
//
typedef struct {
    Byte serviceType;            // Service Type(P/D): 0x00: Program, 0x80: Data.
    Dword serviceId;             // Service ID.
    Word componentId;            // Stream audio/data is subchid, packet mode is SCId.
    Byte componentIdService;     // Component ID within Service.
    DTVLabel componentLabel;     // The label of component. See the defination of Label.
    Byte language;               // Language code.
    Byte primary;                // Primary/Secondary.
    Byte conditionalAccess;      // Conditional Access flag.
    Byte componentType;          // Component Type (A/D).
    Byte transmissionId;         // Transmission Mechanism ID.
} DTVComponent, *PDTVComponent;

typedef struct _MODULATION_PARAM{
	Dword IOCTLCode;
    Byte highCodeRate;
    Byte transmissionMode;
    Byte constellation; 
    Byte interval;   
} MODULATION_PARAM, *PMODULATION_PARAM;

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Initialize Chip and Power on device.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_TxDeviceInit(Byte handleNum);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Set the output stream type of chip. Because the device could output in 
//      many stream type, therefore host have to choose one type before receive 
//      data.
//
//  PARAMETERS:
//      streamType - One of DTVStreamType.
//          DVB-T: DTVStreamType_DVBT_DATAGRAM
//          DVB-H: DTVStreamType_DVBH_DATAGRAM
//          TDMB:  DTVStreamType_TDMB_DATAGRAM
//          FM:    DTVStreamType_FM_DATAGRAM
//
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_SetStreamType(
    IN  DTVStreamType streamType);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Clean up & Power off device.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_Finalize(void);

DTVEXPORT Dword g_ITEAPI_TxSetChannelModulation(	
	IN MODULATION_PARAM ChannelModulation_Setting);
	
DTVEXPORT Dword g_ITEAPI_GetModulation(
	Byte* transmissionMode,
	Byte* constellation,
	Byte* interval,
	Byte* highCodeRate);
	
DTVEXPORT Dword g_ITEAPI_TxSetChannel(	
	IN Dword bfrequency,
	IN Word bbandwidth);

DTVEXPORT Dword g_ITEAPI_GetFrequency(
	Dword* frequency,
	Word* bandwidth);

DTVEXPORT Dword g_ITEAPI_SetTxMode(IN Byte OnOff);

DTVEXPORT Dword g_ITEAPI_SetDeviceType(IN Byte DeviceType);

DTVEXPORT Dword g_ITEAPI_GetDeviceType(OUT Byte *DeviceType);

DTVEXPORT Dword g_ITEAPI_AdjustOutputGain(IN int Gain_value, OUT int *Out_Gain_valu);


// -----------------------------------------------------------------------------
//  PURPOSE:
//      Enable Pid Table, for DVB-T mode.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_EnablePIDTbl(void);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Disable Pid Table, for DVB-T mode.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_DisablePIDTbl(void);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Add Pid number, for DVB-T mode.
//
//  PARAMETERS:
//      byIndex - 0 ~ 31.
//      wProgId - pid number.
//
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_AddPID(
    IN  Byte byIndex,
    IN  Word wProgId);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Remove Pid number, for DVB-T mode.
//
//  PARAMETERS:
//      byIndex - 0 ~ 31.
//      wProgId - pid number.
//
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_RemovePID(
    IN  Byte byIndex,
    IN  Word wProgId);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Clear all the PID's set previously by g_ITEAPI_AddPID(), for DVB-T mode.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ResetPIDTable(void);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Add Pid number with extension parameters, specifically for DVB-H.
//
//  PARAMETERS:
//      pid - pid structure data (Specify the PID number and relevant attributes in DVB-H mode).
//
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_AddPIDEx(
    IN  DTVPid pid);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Remove Pid number with extension parameters, specifically for DVB-H.
//
//  PARAMETERS:
//      pid - pid structure data.
//
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_RemovePIDEx(
    IN  DTVPid pid);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Start to Capture data from device, for DVB-T/TDMB/FM mode.
//      If the the channle is locked properly, the drievr starts to receive TS data and
//      store it in the ring buffer.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------

Dword g_ITEAPI_StartTransfer(void);


Dword g_ITEAPI_StartCapture(void);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Stop to Capture data from device, for DVB-T/TDMB/FM mode.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_StopCapture(void);

DTVEXPORT Dword g_ITEAPI_StopTransfer(void);


// -----------------------------------------------------------------------------
//  PURPOSE:
//      Control Power Saving. The function can be called by application for power saving while idle mode.
//
//  PARAMETERS:
//      byCtrl - 1: Power Up, 0: Power Down.
//               Power Up :  Resume  device to normal state.
//               Power Down : Suspend device to hibernation state.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ControlPowerSaving(
    IN  Byte byCtrl);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get Driver & API Version.
//
//  PARAMETERS:
//      pDriverInfo - Return driver information with DTVDriverInfo structure.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_GetDrvInfo(
    OUT PTxDemodDriverInfo pDriverInfo);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get data from driver, for DVB-T/TDMB/FM mode.
//
//  PARAMETERS:
//      pbyBuffer - Data buffer point.
//      pdwBufferLength - IN: Data buffer length, OUT: Data buffer filled length actually.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_GetData(
    OUT Byte* pbyBuffer,
    IN OUT Dword* pdwBufferLength);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get data length of current ring buffer.
//
//  PARAMETERS:
//      pdwLen - Current TS data length.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_GetTSDataBufferLen(
    OUT Dword* pdwLen);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Clean all ring buffer data in driver.
//
//  PARAMETERS:
//      None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_CleanTSDataBuffer(void);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Acquire all DVBH platforms in current frequency.
//
//  PARAMETERS:
//      pbyPlatformLength - IN: Allocated platform number, OUT: Return channel platform number. (Max platform number is 9)
//      pPlatforms - Platforms array buffer.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
/*DTVEXPORT Dword g_ITEAPI_AcquirePlatform(
    IN OUT Byte* pbyPlatformNumber,
    OUT PDTVPlatform pPlatforms);*/

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Set the current DVBH platform as the specified platform.
//
//  PARAMETERS:
//      pPlatform - Platform structure data to be set.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_SetPlatform(
    IN PDTVPlatform pPlatform);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get SI/PSI & Table data from driver, specifically for DVB-H mode.
//
//  PARAMETERS:
//      pwBufferLength - IN: Allocated buffer length, OUT: Return section data length.
//      pBuffer - Section data buffer point.
//
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_GetSection(
    IN OUT Word* pwBufferLength,
	OUT Byte* pBuffer);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Read OFDM register.
//
//  PARAMETERS:
//      dwRegAddr - Register address.
//      pbyData - Register value.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ReadRegOFDM(
    IN  Dword dwRegAddr,
    OUT Byte* pbyData);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Write OFDM register.
//
//  PARAMETERS:
//      dwRegAddr - Register address.
//      byData - Register value.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_WriteRegOFDM(
    IN  Dword dwRegAddr,
    IN  Byte byData);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Read LINK register.
//
//  PARAMETERS:
//      dwRegAddr - Register address.
//      pbyData - Register value.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ReadRegLINK(
    IN  Dword dwRegAddr,
    OUT Byte* pbyData);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Write LINK register.
//
//  PARAMETERS:
//      dwRegAddr - Register address.
//      byData - Register value.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_WriteRegLINK(
    IN  Dword dwRegAddr,
    IN  Byte byData);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Read EEPROM I2C address from hardware.
//
//  PARAMETERS:
//      byData - I2C address.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ReadEEPROMI2CAddr(
    OUT Byte* pbyData);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Read EEPROM data.
//
//  PARAMETERS:
//      wRegAddr - Register address.
//      pbyData - Register value.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ReadEEPROM(
    IN  Word wRegAddr,
    OUT Byte* pbyData);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get Gain Range.
//
//  PARAMETERS:
//      frequency, bandwidth, MaxGain, MinGain
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_GetGainRange(
	IN Dword frequency,
	IN Word bandwidth,
	OUT int *MaxGain,
	OUT int *MinGain);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get TPS.
//
//  PARAMETERS:
//      TPS structure
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_TxGetTPS(
	OUT TPS *tps);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Set TPS.
//
//  PARAMETERS:
//      TPS structure
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_TxSetTPS(
	IN TPS tps);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get Output Gain.
//
//  PARAMETERS:
//      gain
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxGetOutputGain(
	OUT int *gain);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get Chip Version. 
//
//  PARAMETERS:
//      ChipType - Chip version id
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxGetChipType(
	OUT uint32_t *ChipType);	

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Get Number of Device. 
//
//  PARAMETERS:
//      NumOfDev - Number of Device.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxGetNumOfDevice(
	OUT Byte *NumOfDev);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      g_ITEAPI_TxSendHwPSITable: Send Hardware PSI Table. 
//
//  PARAMETERS:
//      bufferSize - size of TableBuffer.
//		 TableBuffer
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxSendCustomPacketOnce(
	IN int bufferSize,
	IN Byte *TableBuffer);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      g_ITEAPI_TxAccessFwPSITable: Send Hardware PSI Table. 
//
//  PARAMETERS:
//      bufferSize - size of TableBuffer.
//		 TableBuffer
//		 index - PSI Table Index.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxSetPeridicCustomPacket(
	IN int bufferSize,
	IN Byte *TableBuffer,
	IN Byte index);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      g_ITEAPI_TxSetFwPSITableTimer: Send Hardware PSI Table.
//       
//  PARAMETERS:
//		 index - PSI Table Index.
//		 timer_interval- timer.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxSetPeridicCustomPacketTimer(
	IN Byte index, 
	IN Byte timer_interval);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Write Data. 
//
//  PARAMETERS:
//		 pBuffer.
//		 pdwBufferLength.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxSendTSData(
    OUT Byte* pBuffer,
    IN OUT Dword pdwBufferLength);
    
// -----------------------------------------------------------------------------
//  PURPOSE:
//      Load IQ Table From File. 
//
//  PARAMETERS:
//		 None.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
Dword g_ITEAPI_TxLoadIQTableFromFile(
	void);

// -----------------------------------------------------------------------------
//  PURPOSE:
//      Read OFDM register.
//
//  PARAMETERS:
//      dwRegAddr - Register address.
//      pbyData - Register value.
// 
//  RETURNS:
//      0 if no error, non-zero value otherwise.
// -----------------------------------------------------------------------------
DTVEXPORT Dword g_ITEAPI_ReadRegOFDM(
    IN  Dword dwRegAddr,
    OUT Byte* pbyData);

#endif  

