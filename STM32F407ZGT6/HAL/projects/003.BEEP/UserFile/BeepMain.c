#include "stm32f4xx_hal.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"

void DeviceKeyLeftEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    DeviceBeepSet(BEEP_ON);
    HAL_Delay(100);
}

void DeviceKeyRightEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    HAL_Delay(100);
    DeviceBeepSet(BEEP_OFF);
}

void KeyScanLoop(void)
{
    KEY_STATUS keyStatus = KEY_RELEASE;
    keyStatus = DeviceKeyRead(KEY_LEFT);
    if (keyStatus == KEY_PRESS)
    {
        DeviceKeyLeftEvent();
        return;
    }
    keyStatus = DeviceKeyRead(KEY_RIGHT);
    if (keyStatus == KEY_PRESS)
    {
        DeviceKeyRightEvent();
        return;
    }
    HAL_Delay(100);
}


int main(void)
{
    /**HAL库初始化 */
    HAL_Init();
    /**LED初始化 */
    DeviceLedInit(LED_STATUS_OFF);
    /**按键初始化 */
    DeviceKeyInit();
    /**Beeo初始化 */
    DeviceBeepInit(BEEP_OFF);
    while(1)
    {
        KeyScanLoop();
    }
}