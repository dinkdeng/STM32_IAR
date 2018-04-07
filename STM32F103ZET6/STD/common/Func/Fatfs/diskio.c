/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "DeviceSD.h"
#include "SystemUtil.h"

#define DEV_SDCARD			0
#define DEV_W25QXX			1


/**获取驱动器状态 */
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv)
	{
	case DEV_SDCARD :
		return 0;
	case DEV_W25QXX :
		return 0;
	}
	return STA_NOINIT;
}



/**初始化驱动器 */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SDCARD :
		result = DeviceSD_Init();
		if(result)
		{
			DeviceSD_SpeedLow();
			DeviceSD_ReadWriteByte(0xff);
			DeviceSD_SpeedHigh();
		}
		if(result == 0)
			stat = 0;
		else
			stat = STA_NOINIT;
		return stat;
	case DEV_W25QXX :
		return STA_NOINIT;
	}
	return STA_NOINIT;
}



/**读取驱动器指定扇区 */
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int result;
	switch (pdrv)
	{
	case DEV_SDCARD :
		result=DeviceSD_ReadBlocks((uint8_t*)buff,sector,count);
		/**STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常 */
		if(result)
		{
			DeviceSD_SpeedLow();
			DeviceSD_ReadWriteByte(0xff);
			DeviceSD_SpeedHigh();
		}
		if(result==0x00)
			return RES_OK;
    	else
			return RES_ERROR;
	case DEV_W25QXX :
		return RES_PARERR;
	}

	return RES_PARERR;
}



/**写入指定扇区 */
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	int result;
	switch (pdrv)
	{
	case DEV_SDCARD :
		result=DeviceSD_WriteBlocks((uint8_t*)buff,sector,count);
		/**STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常 */
		if(result)
		{
			DeviceSD_SpeedLow();
			DeviceSD_ReadWriteByte(0xff);
			DeviceSD_SpeedHigh();
		}
		if(result==0x00)
			return RES_OK;
    	else
			return RES_ERROR;
	case DEV_W25QXX :
		return RES_PARERR;
	}

	return RES_PARERR;
}



/**控制驱动器操作 */
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	switch (pdrv)
	{
	case DEV_SDCARD :
		switch(cmd)
	    {
		    case CTRL_SYNC:
				DeviceSD_CS_Set(0);
		        if(DeviceSD_WaitReady()==0)
					res = RES_OK;
		        else
					res = RES_ERROR;
				DeviceSD_CS_Set(1);
				return res;
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        return res;
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        return res;
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = DeviceSD_GetSectorCount();
		        res = RES_OK;
		        return res;
		    default:
		        res = RES_PARERR;
		        return res;
	    }
	case DEV_W25QXX :
		return RES_PARERR;
	}
	return RES_PARERR;
}

