#include "stm32f1xx.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"


int main(void)
{
    /*HAL库初始化,初始化系统时钟*/
    HAL_Init();
    /**延时函数初始化 */
    CoreTickDelayInit();
    /**LED初始化 */
    DeviceLedInit(LED_STATUS_OFF);
    while(1)
    {
        DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
        CoreTickDelayMs(50);
        DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
        CoreTickDelayMs(50);
        DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
        CoreTickDelayMs(50);
        DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
        CoreTickDelayMs(50);
    }
}