#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "CoreTimer3Ch2PWM.h"

int timer3Ch2Duty = 1;


void CoreTimer3Ch2PwmControl()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        timer3Ch2Duty+=5;
        if(timer3Ch2Duty > 98)
            timer3Ch2Duty = 98;
        printf("Duty++ :%d\r\n",timer3Ch2Duty);
        CoreTimer3Ch2PwmSetDuty(timer3Ch2Duty);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        timer3Ch2Duty -= 5;
        if(timer3Ch2Duty < 1)
            timer3Ch2Duty = 1;
        printf("Duty-- :%d\r\n",timer3Ch2Duty);
        CoreTimer3Ch2PwmSetDuty(timer3Ch2Duty);
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
    /**外部按键初始化*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_RIGHT);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    /**初始化Timer3*/
    CoreTimer3Ch2PwmInit(99,71,1,0);
    while(1)
    {
        CoreTimer3Ch2PwmControl();
        recvLength = CoreSerialUart1DmaGetRecv(&uart1RecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uart1RecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
        CoreTickDelayMs(1000);
    }
}