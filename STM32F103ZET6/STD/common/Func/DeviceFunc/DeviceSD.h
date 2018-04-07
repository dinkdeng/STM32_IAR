#ifndef __DEVICE_SD_H_
#define __DEVICE_SD_H_
#include "stm32f10x.h"

#define DEVICE_SD_SPI_SPEED_LOW         CORE_SPI_SPEED_256
#define DEVICE_SD_SPI_SPEED_HIGH        CORE_SPI_SPEED_2

#define DEVICE_SD_SPI_CPOL              CORE_SPI_CPOL_HIGH
#define DEVICE_SD_SPI_CPHA              CORE_SPI_CPHA_2EDGE

#define DeviceSD_CS_Set(n)                  (PDout(2) = n)

// SD卡类型定义  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

extern uint8_t  SD_Type;

/**卡片初始化,返回卡片类型 */
uint8_t DeviceSD_Init(void);

/**获取卡片包含的扇区总数 */
uint32_t DeviceSD_GetSectorCount(void);

/**读取数据块 */
uint8_t DeviceSD_ReadBlocks(uint8_t* buffer,uint32_t sector,uint8_t count);

/**写入数据块 */
uint8_t DeviceSD_WriteBlocks(uint8_t* buffer,uint32_t sector,uint8_t count);

/**获取CID信息 */
uint8_t DeviceSD_Get_CID_Info(uint8_t* cid_data);

/**获取CSD信息 */
uint8_t DeviceSD_Get_CSD_Info(uint8_t* csd_data);


/**支援FATFS */
uint8_t DeviceSD_ReadWriteByte(u8 data);
void DeviceSD_SpeedLow(void);
//SD卡正常工作的时候,可以高速了
void DeviceSD_SpeedHigh(void);
uint8_t DeviceSD_WaitReady(void);

#endif




