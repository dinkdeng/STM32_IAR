#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "CoreRand.h"
#include "CoreWakeup.h"


int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    /**初始化WakeUp */
    CoreWakeupInit();
    /**蜂鸣器初始化 */
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1DMA_Init(115200,CoreSerialUart1DMA_DefaultCallBack);
    /**随机数发生器初始化*/
    CoreRandInit();
    printf("System Init Over\r\n");
    while(1)
    {
        for(uint32_t i = 0; i < 20; i++)
        {
            DeviceLedToogle(LED_INDEX_RED);
            CoreTickDelayMs(100);
        }
        CoreWakeUpEnterStandBy();
    }
}




