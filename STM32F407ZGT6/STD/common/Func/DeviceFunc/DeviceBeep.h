#ifndef __DEVICE_BEEP_H_
#define __DEVICE_BEEP_H_
#include "stm32f4xx.h"


typedef enum BEEP_STATUS
{
    BEEP_OFF = 0,
    BEEP_ON = 1,
}BEEP_STATUS;

void DeviceBeepInit(BEEP_STATUS initStatus);

void DeviceBeepSet(BEEP_STATUS set);



#endif






