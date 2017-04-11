#ifndef _FLASH_H_
#define _FLASH_H_

#include <FeeLib.h>

void WriteToFlash(long *pArray, unsigned long ulStartAddress, unsigned int uiSize);
void ReadFromFlash(long *pArray, unsigned long ulStartAddress, unsigned int uiSize);

#endif
