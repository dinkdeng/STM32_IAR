#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "CoreADC.h"

void CoreADCTest()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        uint16_t coreAdcValue = 0;
        coreAdcValue = CoreADCGetValueBychannel(ADC_Channel_1,10);
        printf("ADC Value :%d  Vol Value : %0.3f\r\n",coreAdcValue,(coreAdcValue/4096.0)*3.3);

    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
    }
}

int main(void)
{
    uint8_t* uartRecvDatBufferPtr = NULL;
    uint16_t recvLength = 0;

    /*初始化系统分组*/
    SystemUtilSetIntGroup();
    /*初始化延时函数*/
    CoreTickDelayInit();
    /**外部按键初始化*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_RIGHT);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    /**初始化ADC*/
    CoreADCInit();
    while(1)
    {
        CoreADCTest();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}