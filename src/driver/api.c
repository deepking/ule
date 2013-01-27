// DTVAPI.cpp : Defines the entry point for the DLL application.
//

#include "api.h"

int g_hDriver = 0;
int g_hSPIDriverHandle = 0;
int g_hPrimaDriverHandle = 0;



//===========================================================================
// The client calls this function to get SDIO/USB/SPI device handle.
// @param void
// Return:  int if successful, INVALID_HANDLE_VALUE indicates failure.
//===========================================================================
int g_ITEAPI_GetDriverHandle(Byte handleNum)
{
	int hDriver = 0;
	char* devName = "";
	int ret;
	
	ret = asprintf(&devName, "/dev/usb-it950x%d", handleNum);
    hDriver = open(devName, O_RDWR);

	if (hDriver > 0)
		printf("\nOpen /dev/usb-it950x%d ok\n", handleNum);
	else
		printf("\nOpen /dev/usb-it950x%d fail\n", handleNum);

	return hDriver;
}


//===========================================================================
// The client calls this function to close Af9015 device.
// @param   Af9015 handle
// Return:  TRUE indicates success. FALSE indicates failure. 
//===========================================================================
int g_ITEAPI_CloseDriverHandle(
    IN  int hObject)
{
    //here tell driver stop to read data
    //g_ITEAPI_StopCapture();

    return (close(hObject));
}


Dword g_ITEAPI_TxDeviceInit(Byte handleNum)
{
    Dword dwError = ERR_NO_ERROR;
    TxDemodDriverInfo DriverInfo;
	
    g_hDriver = g_ITEAPI_GetDriverHandle(handleNum);

    //g_ITEAPI_ControlPowerSaving(1);

    // Check driver is loaded correctly
    dwError = g_ITEAPI_GetDrvInfo(&DriverInfo);

    if (g_hDriver == INVALID_HANDLE_VALUE)
        dwError = ERR_INVALID_DEV_TYPE;

	return (dwError);
}


Dword g_ITEAPI_Finalize()
{
    Dword dwError = ERR_NO_ERROR;

    //g_ITEAPI_ControlPowerSaving(0);
    //g_ITEAPI_ResetPIDTable();
    //g_ITEAPI_DisablePIDTbl();
	//g_ITEAPI_SetTxMode(0);

    if (g_hDriver)
        g_ITEAPI_CloseDriverHandle(g_hDriver);
	
    return (dwError);
}


//
// ucCtrl ( 1 : Power Up, 0 : Power Down )
//
Dword g_ITEAPI_ControlPowerSaving(
    IN  Byte byCtrl)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    TxControlPowerSavingRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.control = byCtrl;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_CONTROLPOWERSAVING_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }
	
    return dwError;
}

Dword g_ITEAPI_TxSetChannelModulation(	
	IN MODULATION_PARAM ChannelModulation_Setting
)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    SetModuleRequest request;
	Byte transmissionMode = ChannelModulation_Setting.transmissionMode;
	Byte constellation = ChannelModulation_Setting.constellation;
	Byte interval = ChannelModulation_Setting.interval;
	Byte highCodeRate = ChannelModulation_Setting.highCodeRate;

	if (g_hDriver > 0) {
        request.chip = 0;
		request.transmissionMode = transmissionMode;
        request.constellation = constellation;
		request.interval = interval;
		request.highCodeRate = highCodeRate;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_SETMODULE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_GetModulation(
	Byte* transmissionMode,
	Byte* constellation,
	Byte* interval,
	Byte* highCodeRate
)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    SetModuleRequest request;
	if (g_hDriver > 0) {
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETMODULE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}
	
	*transmissionMode = request.transmissionMode;
	*constellation = request.constellation;
	*interval = request.interval;
	*highCodeRate = request.highCodeRate;
	
    return (dwError);
}

Dword g_ITEAPI_TxSetChannel(	
	IN Dword bfrequency,
	IN Word bbandwidth)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    TxAcquireChannelRequest request;

	if (g_hDriver > 0) {
        request.chip = 0;
		request.frequency = bfrequency;
        request.bandwidth = bbandwidth;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_GetFrequency(
	Dword* frequency,
	Word* bandwidth
)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    GetAcquireChannelRequest request;

	if (g_hDriver > 0) {
        request.chip = 0;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GET_ACQUIRECHANNEL_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}
	*frequency = request.frequency;	
	*bandwidth = request.bandwidth;
    return (dwError);
}

Dword g_ITEAPI_SetTxMode(IN Byte OnOff)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    TxModeRequest request;

	if (g_hDriver > 0) {
        request.OnOff = OnOff;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_ENABLETXMODE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_SetDeviceType(IN Byte DeviceType)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    TxModeRequest request;

	if (g_hDriver > 0) {
		request.OnOff = DeviceType;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_SETDEVICEMODE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_GetDeviceType(OUT Byte *DeviceType)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    TxModeRequest request;

	if (g_hDriver > 0) {
		request.OnOff = 1;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETDEVICEMODE_TX, (void *)&request);
        *DeviceType = request.OnOff;
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_AdjustOutputGain(IN int Gain_value, OUT int *Out_Gain_value)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    SetGainRequest request;
    
	if (g_hDriver > 0) {
		request.GainValue = Gain_value;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX, (void *)&request);
        *Out_Gain_value = request.GainValue;
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_EnablePIDTbl(void)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    TxControlPidFilterRequest request;

    //RETAILMSG( 1, (TEXT("g_ITEAPI_EnablePIDTbl\n\r") ));

    if (g_hDriver > 0) {
        request.chip = 0;
        request.control = 1;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_CONTROLPIDFILTER_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return(dwError);
}


Dword g_ITEAPI_DisablePIDTbl(void)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    TxControlPidFilterRequest request;

    //RETAILMSG( 1, (TEXT("g_ITEAPI_DisablePIDTbl\n\r") ));

    if (g_hDriver > 0) {
        request.chip = 0;
        request.control = 0;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_CONTROLPIDFILTER_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return(dwError);
}


Dword g_ITEAPI_AddPID(
    Byte byIndex,            // 0 ~ 31
	Word wProgId)            // pid number
{	
    Dword dwError = ERR_NO_ERROR;
    int result;
    AddPidAtRequest request;
    Pid pid;
	memset(&pid, 0, sizeof(pid));
    pid.value = (Word)wProgId;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.pid = pid;
        request.index = byIndex;

        //RETAILMSG( 1, (TEXT("g_ITEAPI_AddPID - Index = %d, Value = %d\n\r"), request.index, request.pid.value));

        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_ADDPIDAT_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return (dwError);
}


Dword g_ITEAPI_RemovePID(
    Byte byIndex,            // 0 ~ 31
	Word wProgId)            // pid number
{	
    Dword dwError = ERR_NO_ERROR;
    int result;
    RemovePidAtRequest request;
    Pid pid;
	memset(&pid, 0, sizeof(pid));
    pid.value = (Word)wProgId;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.pid = pid;
        request.index = byIndex;

        //RETAILMSG( 1, (TEXT("g_ITEAPI_AddPID - Index = %d, Value = %d\n\r"), request.index, request.pid.value));

        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_REMOVEPIDAT_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return (dwError);
}


Dword g_ITEAPI_ResetPIDTable()
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    ResetPidRequest request;

    //RETAILMSG( 1, (TEXT("g_ITEAPI_ResetPIDTable\n\r")));

    if (g_hDriver > 0) {
        request.chip = 0;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_RESETPID_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return(dwError);
}


Dword g_ITEAPI_SetStreamType(
    IN  DTVStreamType streamType)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    SetStreamTypeRequest request;

    if (g_hDriver > 0) {
        request.streamType = (StreamType)streamType;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_SETSTREAMTYPE_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return (dwError);
}

Dword g_ITEAPI_StartTransfer(){

	Dword dwError = ERR_NO_ERROR;
    int result;

    //here tell driver begin to read data
    result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_STARTTRANSFER_TX);

    return(dwError);
}

Dword g_ITEAPI_StopTransfer()
{
    Dword dwError = ERR_NO_ERROR;
    int result;

    result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_STOPTRANSFER_TX);

    return(dwError);
}

Dword g_ITEAPI_StartCapture()
{
    Dword dwError = ERR_NO_ERROR;
    int result;

    //here tell driver begin to read data
    result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_STARTCAPTURE);

    return(dwError);
}


Dword g_ITEAPI_StopCapture()
{
    Dword dwError = ERR_NO_ERROR;
    int result;

    result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_STOPCAPTURE);

    return(dwError);
}


Dword g_ITEAPI_GetDrvInfo(
    OUT PTxDemodDriverInfo pDriverInfo)
{
    Dword dwError = ERR_NO_ERROR;
    int result;

    if (g_hDriver > 0) {
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETDRIVERINFO_TX, (void *)pDriverInfo);
        dwError = pDriverInfo->error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;	
}

Dword g_ITEAPI_TxSendTSData(
    OUT Byte* pBuffer,
    IN OUT Dword pdwBufferLength)
{
	Dword dwError = ERR_NO_ERROR;
	int Len = 0;
	
    if (g_hDriver > 0) {
		Len =  write(g_hDriver, pBuffer, pdwBufferLength);
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
        printf("ERRRR\n");
    }

    return Len;
}

Dword g_ITEAPI_GetData(
    OUT Byte* pBuffer,
    IN OUT Dword* pdwBufferLength)
{
    Dword dwError = ERR_NO_ERROR;
    GetDatagramRequest request;

    if (g_hDriver > 0) {
        request.bufferLength = pdwBufferLength;
        request.buffer = pBuffer;
        //result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETDATAGRAM_TX, (void *)&request);
		*pdwBufferLength = read(g_hDriver, pBuffer, *pdwBufferLength);
        dwError = request.error;
        //ReadFile(g_hDriver, pBuffer, *pdwBufferLength, pdwBufferLength, NULL);
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}


Dword g_ITEAPI_GetTSDataBufferLen(
    OUT Dword* pdwLen)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    GetTSBufferLenRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETTSDATABUFFERLEN, (void *)&request);
        dwError = request.error;

        *pdwLen = request.length;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}

Dword g_ITEAPI_CleanTSDataBuffer()
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    CleanTSDataBufferRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_CLEANTSDATABUFFER, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return(dwError);
}

Dword g_ITEAPI_AddPIDEx(
    IN  DTVPid pid)
{	
    Dword dwError = ERR_NO_ERROR;
    int result;
    AddPidAtRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        memcpy(&request.pid, &pid, sizeof(Pid));
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_ADDPIDAT_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return (dwError);
}


Dword g_ITEAPI_RemovePIDEx(
    IN  DTVPid pid)
{	
    Dword dwError = ERR_NO_ERROR;
    int result;
    RemovePidAtRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        memcpy(&request.pid, &pid, sizeof(Pid));
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_REMOVEPIDAT_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return (dwError);
}

Dword g_ITEAPI_ReadRegOFDM(
    IN  Dword dwRegAddr,
    OUT Byte* pbyData)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    TxReadRegistersRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.processor = Processor_OFDM;
        request.registerAddress = dwRegAddr;
        request.bufferLength = 1;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_READREGISTERS_TX, (void *)&request);
		*pbyData = request.buffer[0];
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}


Dword g_ITEAPI_WriteRegOFDM(
    IN  Dword dwRegAddr,
    IN  Byte byData)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    WriteRegistersRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.processor = Processor_OFDM;
        request.registerAddress = dwRegAddr;
        request.bufferLength = 1;
        memcpy (request.buffer, &byData, 1);
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_WRITEREGISTERS_TX, (void *)&request);
        dwError = request.error;
    } else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}


Dword g_ITEAPI_ReadRegLINK(
    IN  Dword dwRegAddr,
    OUT Byte* pbyData)
{
    Dword dwError = ERR_NO_ERROR;

    int result;
    TxReadRegistersRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.processor = Processor_LINK;
        request.registerAddress = dwRegAddr;
        request.bufferLength = 1;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_READREGISTERS_TX, (void *)&request);
		*pbyData = request.buffer[0];
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}


Dword g_ITEAPI_WriteRegLINK(
    IN  Dword dwRegAddr,
    IN  Byte byData)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    WriteRegistersRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.processor = Processor_LINK;
        request.registerAddress = dwRegAddr;
        request.bufferLength = 1;
        memcpy (request.buffer, &byData, 1);
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_WRITEREGISTERS_TX, (void *)&request);
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}


Dword g_ITEAPI_ReadEEPROMI2CAddr(
    OUT Byte* pbyData)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    GetEEPROMI2CAddrRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETEEPROMI2CADDR, (void *)&request);
        dwError = request.error;

        *pbyData = request.I2CAddr;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}


Dword g_ITEAPI_ReadEEPROM(
    IN  Word wRegAddr,
    OUT Byte* pbyData)
{
    Dword dwError = ERR_NO_ERROR;
    int result;
    TxReadEepromValuesRequest request;

    if (g_hDriver > 0) {
        request.chip = 0;
        request.registerAddress = wRegAddr;
        request.bufferLength = 1;
        result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_READEEPROMVALUES_TX, (void *)&request);
		*pbyData = request.buffer[0];
        dwError = request.error;
    }
    else {
        dwError = ERR_NOT_IMPLEMENTED;
    }

    return dwError;
}

Dword g_ITEAPI_GetGainRange(IN Dword frequency, IN Word bandwidth, OUT int *MaxGain, OUT int *MinGain)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    GetGainRangeRequest request;

	if (g_hDriver > 0) {
		if(frequency >= 474000 && frequency <= 9000000)
			request.frequency = frequency;
		else {
			printf("\nSet Frequency Out of Range!\n");
			dwError = ERR_FREQ_OUT_OF_RANGE;
		}
		if(bandwidth >= 6000 && bandwidth <= 8000)
			request.bandwidth = bandwidth;
		else {
			printf("\nSet Bandwidth Out of Range!\n");
			dwError = ERR_INVALID_BW;
		}
		if(!dwError) {
			request.maxGain = (int*) MaxGain;
			request.minGain = (int*) MinGain;        		
			result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETGAINRANGE_TX, (void *)&request);
			dwError = request.error;
		}
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_TxGetTPS(OUT TPS *tps)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    GetTPSRequest request; 
      
	if (g_hDriver > 0) {
		request.pTps = tps;
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETTPS_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_TxSetTPS(IN TPS tps)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    SetTPSRequest request; 
      
	if (g_hDriver > 0) {
		request.tps = tps;
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_SETTPS_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_TxGetOutputGain(OUT int *gain)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    GetOutputGainRequest request; 
      
	if (g_hDriver > 0) {
		request.gain = (int*) gain;
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}

    return (dwError);
}

Dword g_ITEAPI_TxGetChipType(OUT uint32_t *ChipType)
{
	Dword dwError = ERR_NO_ERROR;
	uint8_t value1, value2;
    uint32_t regAddr = 0x1223;
      
	dwError = g_ITEAPI_ReadRegLINK((Word)regAddr, &value1);
	dwError = g_ITEAPI_ReadRegLINK((Word)regAddr+1, &value2);	
	*ChipType = (value2 << 8) | value1;
	
    return (dwError);
}

Dword g_ITEAPI_TxGetNumOfDevice(OUT Byte *NumOfDev)
{
	Dword dwError = ERR_NO_ERROR;
    Byte DevCount = 0;
    struct dirent *ptr;
    char *handle = "usb-it950x";
    char *existing;
    DIR *dir = opendir("/dev");
    
    while((ptr = readdir(dir)) != NULL) {
		existing = strndup(ptr->d_name, 10);
		if(!strcmp(existing, handle))
			DevCount++;
	}
	*NumOfDev = DevCount;
	closedir(dir);
    return (dwError);
}

Dword g_ITEAPI_TxSendCustomPacketOnce(IN int bufferSize, IN Byte *TableBuffer)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    SendHwPSITableRequest request; 

	if(bufferSize != 188)
		return -1;

	if (g_hDriver > 0) {
		request.pbuffer = TableBuffer;
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_SENDHWPSITABLE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}
    return (dwError);
}
 
Dword g_ITEAPI_TxSetPeridicCustomPacket(IN int bufferSize, IN Byte *TableBuffer, IN Byte index)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    AccessFwPSITableRequest request; 

	if(bufferSize != 188)
		return -1;

	if (g_hDriver > 0) {
		request.psiTableIndex = index;
		request.pbuffer = TableBuffer;
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_ACCESSFWPSITABLE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}
    return (dwError);
}

Dword g_ITEAPI_TxSetPeridicCustomPacketTimer(IN Byte index, IN Byte timer_interval)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    SetFwPSITableTimerRequest request; 

	if (g_hDriver > 0) {
		request.psiTableIndex = index;
		request.timer = timer_interval;
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_SETFWPSITABLETIMER_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}
    return (dwError);
}

Dword g_ITEAPI_TxLoadIQTableFromFile(void)
{
	Dword dwError = ERR_NO_ERROR;
    int result;
    TxLoadIQTableRequest request; 

	if (g_hDriver > 0) {
		result = ioctl(g_hDriver, IOCTL_ITE_DEMOD_LOADIQTABLE_TX, (void *)&request);
        dwError = request.error;
	}
	else {
        dwError = ERR_NOT_IMPLEMENTED;
	}
    return (dwError);
}
