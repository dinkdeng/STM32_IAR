#ifndef __DEVICE_PLUSE_CAPTURE_H_
#define __DEVICE_PLUSE_CAPTURE_H_
#include "stm32f10x.h"

#define DEVICE_PLUSE_CAPTURE_PREE_PRI   INT_PRE_PRI_1

#define DEVICE_PLUSE_CAPTURE_SUB_PRI    INT_SUB_PRI_0

/*设备初始化*/
void  DevicePluseCaptureInit();

/*获取捕获次数以及最大的捕获时间*/
uint32_t DevicePluseCaptureGetCountWithTime(uint32_t* maxTimer);



#endif



