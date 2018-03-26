#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "CoreSerial1.h"

void DeviceKeyLeftEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    CoreSerial1SendString("DeviceKeyLeftEvent\r\n");
    CoreTickDelayMs(100);
}

void DeviceKeyRightEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    CoreSerial1SendString("DeviceKeyRightEvent\r\n");
    CoreTickDelayMs(100);
}

void DeviceKeyUpEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    CoreSerial1SendString("DeviceKeyUpEvent\r\n");
    CoreTickDelayMs(100);
}

void DeviceKeyDownEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    CoreSerial1SendString("DeviceKeyDownEvent\r\n");
    CoreTickDelayMs(100);
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
    if(CORE_UART1_CHECK_RX_STATUS_LF() != 0)
    {
        CoreSerial1SendBuffer(coreSerial1RXBuffer,coreSerial1RxStatus&0x00FF);
        CORE_UART1_CLEAR_STATUS();
    }
    CoreTickDelayMs(100);
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
    /**初始化串口 */
    CoreSerial1Init(115200);
    while(1)
    {
        KeyScanLoop();
    }
}