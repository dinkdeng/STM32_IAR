#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    DeviceKeyInit();
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1DMA_Init(115200,CoreSerialUart1DMA_DefaultCallBack);
    //printf("System Init Over\r\n");
    CoreSerialUart1DMA_SendString("System Init Over1\r\n");
    CoreSerialUart1DMA_SendString("System Init Over2\r\n");
    while(1)
    {
        printf("System Running\r\n");
        CoreTickDelayMs(500);
    }
}




