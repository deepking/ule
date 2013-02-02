#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>
uint32_t crc32(const unsigned char *pBuffer, int nLength);
uint32_t crc32ForUle(const unsigned char* pHeader, const unsigned char *pBuffer, int nLength);
#endif 
