#ifndef __DEVICE_SRAM_H_
#define __DEVICE_SRAM_H_
#include "stm32f10x.h"

/*SRAM基础地址*/
#define DEVICE_SRAM_BASE_ADDR       0x68000000

/*sram长度*/
#define DEVICE_SRAM_LENGTH          (1024*1024)

/**设备挂载的SRAM初始化 */
uint8_t DeviceSRAMInit(void);

/**设备挂载的SRAM测试,参数为测试步长 */
uint8_t DeviceSRAMTest(uint16_t testStep);


#endif




