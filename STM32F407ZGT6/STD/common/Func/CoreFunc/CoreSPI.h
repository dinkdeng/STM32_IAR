#ifndef __CORE_SPI_H_
#define __CORE_SPI_H_
#include "stm32f4xx.h"

/**定义速度设置需要使用的变量 */
#define CORE_SPI_SPEED_2                SPI_BaudRatePrescaler_2
#define CORE_SPI_SPEED_4                SPI_BaudRatePrescaler_4
#define CORE_SPI_SPEED_8                SPI_BaudRatePrescaler_8
#define CORE_SPI_SPEED_16               SPI_BaudRatePrescaler_16
#define CORE_SPI_SPEED_32               SPI_BaudRatePrescaler_32
#define CORE_SPI_SPEED_64               SPI_BaudRatePrescaler_64
#define CORE_SPI_SPEED_128              SPI_BaudRatePrescaler_128
#define CORE_SPI_SPEED_256              SPI_BaudRatePrescaler_256

#define CORE_SPI_CPOL_HIGH              SPI_CPOL_High
#define CORE_SPI_CPOL_LOW               SPI_CPOL_Low

#define CORE_SPI_CPHA_1EDGE             SPI_CPHA_1Edge
#define CORE_SPI_CPHA_2EDGE             SPI_CPHA_2Edge

#define CORE_SPI_WAIT_COUNT_MAX         50000


/**SPI1初始化 */
void CoreSPI1_Init(uint16_t cpol,uint16_t cpha,uint16_t speed);

/**SPI1读写 */
uint8_t CoreSPI1_WriteRead(uint8_t writeDat);

/**SPI1设置速度 */
void CoreSPI1_SetSpeed(uint16_t speed);

/**SPI1设置CPOL CPHA特性 */
void CoreSPI1_Set_CPXX(uint16_t cpol,uint16_t cpha);




#endif





