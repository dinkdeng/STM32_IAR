#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    while(1)
    {
        CoreTickDelayMs(200);
        DeviceLedSet(LED_INDEX_RED,LED_STATUS_ON);
        CoreTickDelayMs(200);
        DeviceLedSet(LED_INDEX_RED,LED_STATUS_OFF);
        CoreTickDelayMs(200);
        DeviceLedSet(LED_INDEX_GREEN,LED_STATUS_ON);
        CoreTickDelayMs(200);
        DeviceLedSet(LED_INDEX_GREEN,LED_STATUS_OFF);
    }
}




