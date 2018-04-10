#ifndef __DEVICE_SRAM_H_
#define __DEVICE_SRAM_H_
#include "stm32f4xx.h"

/**ISWV51216 512*16/2 1M字节*/

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
/*SRAM基础地址*/
#define DEVICE_SRAM_BASE_ADDR       ((uint32_t)(0x68000000))

/*sram长度*/
#define DEVICE_SRAM_LENGTH          (1024*1024)

/**设备挂载的SRAM初始化 */
uint8_t DeviceSRAM_Init(void);

/**设备挂载的SRAM测试,参数为测试步长 */
uint8_t DeviceSRAM_Test(uint16_t testStep);



#endif




