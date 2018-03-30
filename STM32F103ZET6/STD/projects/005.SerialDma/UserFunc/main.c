#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "CoreSerialUart1Dma.h"

/**LEFT按键处理程序 */
void DeviceKeyLeftEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    CoreSerialUart1DmaSendString("Left Key Press\r\n");
}

/**Right按键处理程序 */
void DeviceKeyRightEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    CoreSerialUart1DmaSendString("Right Key Press\r\n");
}

/**up按键处理程序 */
void DeviceKeyUpEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    CoreSerialUart1DmaSendString("Up Key Press\r\n");
}

/**down按键处理程序 */
void DeviceKeyDownEvent(void)
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    CoreSerialUart1DmaSendString("Down Key Press\r\n");
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
    uint8_t* uart1RecvDatBufferPtr = NULL;
    uint16_t recvLength = 0;
    /*初始化系统分组*/
    SystemUtilSetIntGroup();
    /*初始化延时函数*/
    CoreTickDelayInit();
    /*初始化Led*/
    DeviceLedInit(LED_STATUS_OFF);
    /*初始化外部按键*/
    DeviceKeyInit();
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    while(1)
    {
        KeyScanLoop();
        recvLength = CoreSerialUart1DmaGetRecv(&uart1RecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uart1RecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}