#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"


void DeviceKeyLeftEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    DeviceBeepSet(BEEP_ON);
    CoreTickDelayMs(100);
}

void DeviceKeyRightEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    CoreTickDelayMs(100);
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
    CoreTickDelayMs(100);
}

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    DeviceKeyInit();
    DeviceBeepInit(BEEP_OFF);
    while(1)
    {
        KeyScanLoop();
    }
}




