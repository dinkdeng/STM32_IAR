#include "stm32f4xx.h"
#include "DeviceLed.h"

int main(void)
{
    HAL_Init();
    DeviceLedInit(LED_STATUS_OFF);
    while(1)
    {
        DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
        HAL_Delay(500);
        DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
        HAL_Delay(500);
        DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
        HAL_Delay(500);
        DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
        HAL_Delay(500);
    }
}

