#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"

int main(void)
{
    /*初始化系统分组*/
    SystemUtilSetIntGroup();
    /*初始化延时函数*/
    CoreTickDelayInit();
    /*初始化Led*/
    DeviceLedInit(LED_STATUS_OFF);
    CoreTickDelayMs(5000);
    while(1)
    {
        CoreTickDelayMs(100);
        DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
        CoreTickDelayMs(100);
        DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
        CoreTickDelayMs(100);
        DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
        CoreTickDelayMs(100);
        DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    }
}