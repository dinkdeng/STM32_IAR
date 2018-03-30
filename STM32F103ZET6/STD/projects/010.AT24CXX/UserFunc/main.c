#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "CoreWWDG.h"
#include "DeviceAT24Cxx.h"

void DeviceA24CxxTest(void)
{
    /**读取10字节 */
    uint8_t testBuffer[10] = {0};
    if(DeviceAT24CxxReadBuffer(0,10,testBuffer))
    {
        CoreSerialUart1DmaSendString("Read Buffer Failed\r\n");
        return;
    }
    CoreSerialUart1DmaSendBuffer(testBuffer,10);
    CoreSerialUart1DmaSendString("\r\n");
    for(uint8_t i = 0; i < 10; i++)
    {
        testBuffer[i]++;
    }
    if(DeviceAT24CxxWriteBuffer(0,10,testBuffer))
    {
        CoreSerialUart1DmaSendString("Write Buffer Failed\r\n");
        return;
    }
    if(DeviceAT24CxxReadBuffer(0,10,testBuffer))
    {
        CoreSerialUart1DmaSendString("Read Buffer Failed\r\n");
        return;
    }
    CoreSerialUart1DmaSendBuffer(testBuffer,10);
    CoreSerialUart1DmaSendString("\r\n");
}

/**因为是窗口看门狗,所以随意按键喂狗会造成系统复位,就可以看见
 * 系统偶然复位的现象
 */
void DeviceLeftExtiProcess()
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    DeviceBeepSet(BEEP_OFF);
    DeviceExtiClear(EXTI_LEFT);
    DeviceA24CxxTest();
}

void DeviceRightExtiProcess()
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_OFF);
    DeviceBeepSet(BEEP_ON);
    DeviceExtiClear(EXTI_RIGHT);
    CoreSerialUart1DmaSendString("Right Exti Press\r\n");
}

void DeviceUpExtiProcess()
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_OFF);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    DeviceExtiClear(EXTI_UP);
    CoreSerialUart1DmaSendString("Up Exti Press\r\n");
}

void DeviceDownExtiProcess()
{
    DeviceLedSet(LED_INDEX_RED, LED_STATUS_ON);
    DeviceLedSet(LED_INDEX_GREEN, LED_STATUS_ON);
    DeviceExtiClear(EXTI_DOWN);
    CoreSerialUart1DmaSendString("Down Exti Press\r\n");
}




/**系统按键检测处理 */
void ExtiCheckLoop(void)
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceLeftExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceRightExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceUpExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceDownExtiProcess();
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
    /*初始化EXTI*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_RIGHT);
    DeviceExtiInit(EXTI_UP);
    DeviceExtiInit(EXTI_DOWN);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    /**初始化IIC接口 */
    while(DeviceAT24CxxInit())
    {
        CoreSerialUart1DmaSendString("DeviceAT24CxxInit Filed\r\n");
        CoreTickDelayMs(1000);
    }
    while(1)
    {
        ExtiCheckLoop();
        recvLength = CoreSerialUart1DmaGetRecv(&uart1RecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uart1RecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}