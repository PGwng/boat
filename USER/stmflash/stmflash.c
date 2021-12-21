#include "stmflash.h"

uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}

u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32* )faddr;
}

void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u16 NumToRead)
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);
		ReadAddr+=4;
	}
}
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)
{
	FLASH_Status status=FLASH_COMPLETE;
	u32 endaddr=WriteAddr+4*NumToWrite;
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)
		return;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
	FLASH_DataCacheCmd(DISABLE);	
	
	status=FLASH_EraseSector(STMFLASH_GetFlashSector(WriteAddr),VoltageRange_3);
	if(status==FLASH_COMPLETE)
	{
		while(WriteAddr<endaddr)
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)
			{ 
				break;
			}
			WriteAddr+=4;
			pBuffer++;
		}
	}
	 FLASH_DataCacheCmd(ENABLE);	
	FLASH_Lock();
}














