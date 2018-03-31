#ifndef __CORE_STM32_FLASH_H_
#define __CORE_STM32_FLASH_H_
#include "stm32f10x.h"

/*STM32 FLASH的起始地址*/
#define CORE_STM32_FLASH_BASE           0x08000000

/*所选STM32的FLASH容量大小(单位为K)*/
#define CORE_STM32_FLASH_KB_SIZE 	    512

/*设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)*/
#define CORE_STM32_FLASH_SAVE_ADDR       0X08070000

/**扇区大小 */
#if CORE_STM32_FLASH_KB_SIZE < 256
#define CORE_STM32_FLASH_SECTOR_SIZE    1024
#else
#define CORE_STM32_FLASH_SECTOR_SIZE	2048
#endif

/*写入*/
uint8_t CoreSTM32FlashWrite(uint32_t writeAddr,uint16_t *pBuffer, uint16_t numToWrite);

/*读取*/
uint8_t CoreSTM32FlashRead(uint32_t readAddr,uint16_t *pBuffer, uint16_t numToRead);


#endif





