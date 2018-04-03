#ifndef __DEVICE_NRF24L01_H_
#define __DEVICE_NRF24L01_H_
#include "stm32f10x.h"

/**设备的延时函数 毫秒 */
#define DEVICE_NRF24L01_WAIT_MS(nms)        CoreTickDelayMs(nms)

/**设备的延时函数 微秒 */
#define DEVICE_NRF24L0_WAIT_US(nus)         CoreTickDelayUs(nus)

/**NRF24L01系统性能设置 */
#define DEVICE_NRF24L01_DATA_TRANS_LENGTH       32

/**设备接收地址 */
#define DEVICE_NRF24L01_RX_ADDR             {0x34,0x43,0x10,0x10,0x01}

/**设备发送地址 */
#define DEVICE_NRF24L01_TX_ADDR             {0x34,0x43,0x10,0x10,0x02}

/**设备地址长度 */
#define DEVICE_NRF24L01_ADDR_WIDTH          5

/**接口的信号极性定义 */
#define DEVICE_NRF24L01_CPOL          SPI_CPOL_Low
#define DEVICE_NRF24L01_CPHA          SPI_CPHA_1Edge
/**初始化低速传输 */
#define DEVICE_NRF24L01_SPEED_LOW     CORE_SPI_SPEED_16
/**高速传输,NRF24L01最大速度9MHZ */
#define DEVICE_NRF24L01_SPEED_HIGH    CORE_SPI_SPEED_4

/**24L01片选信号,控制设备是否使能 */
#define DeviceNRF24L01_CE_Set(n)        (PGout(6) = n) 

/**SPI片选信号,控制数据传输是否使能*/
#define DeviceNRF24L01_CSN_Set(n)       (PGout(7) = n)

/**IRQ主机输入 */
#define DeviceNRF24L01_IRQ_Get          PGin(8)




/**设备初始化 */
uint8_t DeviceNRF24L01Init(void);

/**设备设置发送模式 */
void DeviceNRF24L01ModeChangeTX(void);

/**设备设置接受模式 */
void DeviceNRF24L01ModeChangeRX(void);

/**数据包发送 */
uint8_t DeviceNRF24L01SendPackage(uint8_t* sendBufPtr);

/**数据包接收,固定接收系统指定数据长度*/
uint8_t DeviceNRF24L01RecvPackage(uint8_t* recvBuffer);



#endif




