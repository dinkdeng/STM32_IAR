#ifndef __DEVICE_FCPAD_H_
#define __DEVICE_FCPAD_H_
#include "stm32f10x.h"

#define DEVICE_FCPAD_KEY_MASK_A         (1UL<<0)
#define DEVICE_FCPAD_KEY_MASK_B         (1UL<<1)
#define DEVICE_FCPAD_KEY_MASK_SELECT    (1UL<<2)
#define DEVICE_FCPAD_KEY_MASK_START     (1UL<<3)
#define DEVICE_FCPAD_KEY_MASK_UP        (1UL<<4)
#define DEVICE_FCPAD_KEY_MASK_DOWN      (1UL<<5)
#define DEVICE_FCPAD_KEY_MASK_LEFT      (1UL<<6)
#define DEVICE_FCPAD_KEY_MASK_RIGHT     (1UL<<7)

void DeviceFCPAD_Init(void);

uint8_t DeviceFCPAD_ReadKey(void);


#endif





