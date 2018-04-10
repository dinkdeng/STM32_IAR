#ifndef __DEVICE_W25QXX_H_
#define __DEVICE_W25QXX_H_
#include "stm32f4xx.h"

//spi片选引脚定义
//选中FLASH
#define	DeviceW25QXXSetCS(n)	    (PBout(14) = n)

/**接口的信号极性定义 */
#define DEVICE_W25QXX_CPOL          CORE_SPI_CPOL_HIGH
#define DEVICE_W25QXX_CPHA          CORE_SPI_CPHA_2EDGE
#define DEVICE_W25QXX_SPEED         CORE_SPI_SPEED_4

/**ID定义 初始化需要检测ID
 * W25Q80  ID  0XEF13
 * W25Q16  ID  0XEF14
 * W25Q32  ID  0XEF15
 * W25Q64  ID  0XEF16
 * W25Q128 ID  0XEF17
*/
#define DEVICE_W25QXX_ID	        0XEF17

/**数据块数量 */
#define DEVICE_W25QXX_BLOCK_NUM		    256
/**扇区数量,擦除的最小单位是扇区,一个扇区4096字节 */
#define DEVICE_W25QXX_SECTOR_NUM	 	16
/**每个扇区包含的页数*/
#define DEVICE_W25QXX_PAGE_NUM			16
/**每一页包含的字节数*/
#define DEVICE_W25QXX_PAGE_SIZE		    256
/**最大地址*/
#define DEVICE_W25QXX_ADDR_MAX          (DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM*DEVICE_W25QXX_SECTOR_NUM*DEVICE_W25QXX_BLOCK_NUM)

#define DEVICE_W25QXX_WAIT_MS(nms)          CoreTickDelay(nms)

/**芯片初始化 */
uint8_t DeviceW25QXX_Init(void);

/**读取指定位置 */
uint8_t DeviceW25QXX_Read(uint8_t* pBuffer,uint32_t readAddr,uint16_t numByteToRead);

/**写入指定位置 */
uint8_t DeviceW25QXX_Write(uint8_t* pBuffer,uint32_t writeAddr,uint16_t numByteToWrite);			



#endif




