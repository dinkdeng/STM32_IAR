#ifndef __CORE_CHIP_FLASH_H_
#define __CORE_CHIP_FLASH_H_
#include "stm32f4xx.h"

//STM32 FLASH的起始地址
#define CORE_CHIP_FLASH_BASE 0x08000000 	


//设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.
#define CORE_CHIP_SAVE_ADDR  0X0800C004 	

//FLASH 扇区的起始地址 与扇区分布

//扇区0起始地址, 16 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 

//扇区1起始地址, 16 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	

//扇区2起始地址, 16 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 

//扇区3起始地址, 16 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 

//扇区4起始地址, 64 Kbytes
#define CORE_CHIP_ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 

//扇区5起始地址, 128 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 

//扇区6起始地址, 128 Kbytes
#define CORE_CHIP_ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 

//扇区7起始地址, 128 Kbytes  
#define CORE_CHIP_ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 

//扇区8起始地址, 128 Kbytes
#define CORE_CHIP_ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	

//扇区9起始地址, 128 Kbytes
#define CORE_CHIP_ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 

//扇区10起始地址,128 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 

//扇区11起始地址,128 Kbytes 
#define CORE_CHIP_ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 

//读出字 
uint32_t CoreChipFlashReadWord(uint32_t faddr);		  	 

//从指定地址开始写入指定长度的数据
//自动扇区擦除,慎用
uint8_t CoreChipFlashWrite(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);	

//从指定地址开始读出指定长度的数据
uint8_t CoreChipFlashRead(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead); 


#endif





