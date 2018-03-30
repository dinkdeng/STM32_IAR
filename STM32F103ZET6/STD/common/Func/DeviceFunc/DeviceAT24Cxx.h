#ifndef __DEVICE_AT24CXX_H_
#define __DEVICE_AT24CXX_H_
#include "stm32f10x.h"

/*芯片容量定义*/
#define DEVICE_SIZE_AT24C01		    127
#define DEVICE_SIZE_AT24C02		    255
#define DEVICE_SIZE_AT24C04		    511
#define DEVICE_SIZE_AT24C08		    1023
#define DEVICE_SIZE_AT24C16		    2047
#define DEVICE_SIZE_AT24C32		    4095
#define DEVICE_SIZE_AT24C64	        8191
#define DEVICE_SIZE_AT24C128	    16383
#define DEVICE_SIZE_AT24C256	    32767 

/**当前使用的芯片容量定义 */
#define DEVICE_SIZE_AT24CXX_USED    DEVICE_SIZE_AT24C02

/**当前芯片的读写地址定义 */
#define DEVICE_AT24CXX_ADDR_W       0XA0
#define DEVICE_AT24CXX_ADDR_R       0XA1

/**定义是否使用错误信息打印 */
#define DEVICE_AT24CXX_USE_PRINTF   0

/**设备初始化 */
uint8_t DeviceAT24CxxInit(void);

/**读取数据块 */
uint8_t DeviceAT24CxxReadBuffer(uint16_t address,uint16_t length,uint8_t* readBufferPtr);

/**写入数据块 */
uint8_t DeviceAT24CxxWriteBuffer(uint16_t address,uint16_t length,uint8_t* writeBufferPtr);




#endif




