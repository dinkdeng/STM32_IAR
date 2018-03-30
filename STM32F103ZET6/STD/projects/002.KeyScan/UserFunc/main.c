#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"

/**LEFT按键处理程序 */
void DeviceKeyLeftEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
}

/**Right按键处理程序 */
void DeviceKeyRightEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
}

/**up按键处理程序 */
void DeviceKeyUpEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
}

/**down按键处理程序 */
void DeviceKeyDownEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
}
/**系统按键检测处理 */
void KeyScanLoop(void)
{
    KEY_STATUS leftKeyStatus = KEY_RELEASE;
    KEY_STATUS rightKeyStatus = KEY_RELEASE;
    KEY_STATUS upKeyStatus = KEY_RELEASE;
    KEY_STATUS downKeyStatus = KEY_RELEASE;
    leftKeyStatus = DeviceKeyRead(KEY_LEFT);
    rightKeyStatus = DeviceKeyRead(KEY_RIGHT);
    upKeyStatus = DeviceKeyRead(KEY_UP);
    downKeyStatus = DeviceKeyRead(KEY_DOWN);
    if(KEY_PRESS == leftKeyStatus)
    {
        DeviceKeyLeftEvent();
    }
    if(KEY_PRESS == rightKeyStatus)
    {
        DeviceKeyRightEvent();
    }
    if(KEY_PRESS == upKeyStatus)
    {
        DeviceKeyUpEvent();
    }
    if(KEY_PRESS == downKeyStatus)
    {
        DeviceKeyDownEvent();
    }
}

int main(void)
{
    /*初始化系统分组*/
    SystemUtilSetIntGroup();
    /*初始化延时函数*/
    CoreTickDelayInit();
    /*初始化Led*/
    DeviceLedInit(LED_STATUS_OFF);
    /*初始化外部按键*/
    DeviceKeyInit();
    while(1)
    {
        KeyScanLoop();
    }
}