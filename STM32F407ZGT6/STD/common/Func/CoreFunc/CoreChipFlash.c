#include "CoreChipFlash.h"

//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
static uint16_t CoreChipFlashGetFlashSector(uint32_t addr)
{
	if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_1)
		return FLASH_Sector_0;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_2)
		return FLASH_Sector_1;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_3)
		return FLASH_Sector_2;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_4)
		return FLASH_Sector_3;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_5)
		return FLASH_Sector_4;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_6)
		return FLASH_Sector_5;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_7)
		return FLASH_Sector_6;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_8)
		return FLASH_Sector_7;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_9)
		return FLASH_Sector_8;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_10)
		return FLASH_Sector_9;
	else if(addr<CORE_CHIP_ADDR_FLASH_SECTOR_11)
		return FLASH_Sector_10;
    else
	    return FLASH_Sector_11;
}


//读出字
uint32_t CoreChipFlashReadWord(uint32_t faddr)
{
    return *(volatile unsigned int*)faddr;
}

//从指定地址开始写入指定长度的数据
//自动扇区擦除,慎用
//从指定地址开始写入指定长度的数据
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.)
//返回1 写入失败 返回0 写入成功
uint8_t CoreChipFlashWrite(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)
{
    FLASH_Status status = FLASH_COMPLETE;
	uint32_t addrx=0;
	uint32_t endaddr=0;
	//非法地址
	if(WriteAddr<CORE_CHIP_FLASH_BASE||WriteAddr%4)
		return 1;
    /*操作FLASH 写入和擦除的时候必须打开HSI*/
    RCC_HSICmd(ENABLE);
    /*等待HSI稳定*/
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	//解锁
	FLASH_Unlock();
	//FLASH擦除期间,必须禁止数据缓存
	FLASH_DataCacheCmd(DISABLE);

	//写入的起始地址
	addrx=WriteAddr;
	//写入的结束地址
	endaddr=WriteAddr+NumToWrite*4;
	//只有主存储区,才需要执行擦除操作!!
	if(addrx<0X1FFF0000)
	{
		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		while(addrx<endaddr)
		{
			//有非0XFFFFFFFF的地方,要擦除这个扇区
			if(CoreChipFlashReadWord(addrx)!=0XFFFFFFFF)
			{
				//VCC=2.7~3.6V之间!!
				status=FLASH_EraseSector(CoreChipFlashGetFlashSector(addrx),VoltageRange_3);
				//发生错误了
				if(status!=FLASH_COMPLETE)
				{
                    //开启数据缓存
                    FLASH_DataCacheCmd(ENABLE);
                    //上锁
                    FLASH_Lock();
					return 1;
				}
			}
			else
				addrx+=4;
		}
	}
	if(status==FLASH_COMPLETE)
	{
		//写数据
		while(WriteAddr<endaddr)
		{
			//写入数据
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)
			{
                //开启数据缓存
                FLASH_DataCacheCmd(ENABLE);
                //上锁
                FLASH_Lock();
				//写入异常 包括错误操作和写保护
				return 1;
			}
			WriteAddr+=4;
			pBuffer++;
		}
	}
	//FLASH擦除结束,开启数据缓存
	FLASH_DataCacheCmd(ENABLE);
	//上锁
	FLASH_Lock();
	return 0;
}

//从指定地址开始读出指定长度的数据
//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
uint8_t CoreChipFlashRead(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)
{
    uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		//读取4个字节.
		pBuffer[i]=CoreChipFlashReadWord(ReadAddr);
		//偏移4个字节.
		ReadAddr+=4;
	}
	return 0;
}


