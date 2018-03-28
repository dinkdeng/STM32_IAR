#include "stm32f4xx_hal.h"
#include "DeviceLed.h"
#include "DeviceKey.h"


void DeviceKeyLeftEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    HAL_Delay(100);
}

void DeviceKeyRightEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    HAL_Delay(100);
}

void DeviceKeyUpEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    HAL_Delay(100);
}

void DeviceKeyDownEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    HAL_Delay(100);
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
    keyStatus = DeviceKeyRead(KEY_UP);
    if (keyStatus == KEY_PRESS)
    {
        DeviceKeyUpEvent();
        return;
    }
    keyStatus = DeviceKeyRead(KEY_DOWN);
    if (keyStatus == KEY_PRESS)
    {
        DeviceKeyDownEvent();
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
    while(1)
    {
        KeyScanLoop();
    }
}