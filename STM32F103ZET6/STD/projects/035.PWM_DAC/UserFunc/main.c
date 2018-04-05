#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "DevicePWM_DAC.h"
#include "CoreADC.h"

uint16_t pwmDacValue = 0;

void DevicePWM_DAC_Test()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        pwmDacValue += 100;
        if(pwmDacValue > 3300)
            pwmDacValue = 3300;
        DevicePWM_DAC_SetValue(pwmDacValue);
        printf("Set PWM DAC Value : %d\r\n",pwmDacValue);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        pwmDacValue -= 100;
        if(pwmDacValue > 3300)
            pwmDacValue = 0;
        DevicePWM_DAC_SetValue(pwmDacValue);
        printf("Set PWM DAC Value : %d\r\n",pwmDacValue);
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");
        uint16_t coreAdcValue = 0;
        coreAdcValue = CoreADCGetValueBychannel(ADC_Channel_1,10);
        printf("ADC Value :%d  Vol Value : %0.3f\r\n",coreAdcValue,(coreAdcValue/4096.0)*3.3);
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
    DeviceExtiInit(EXTI_UP);
    DeviceExtiInit(EXTI_DOWN);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    /**初始化sram*/
    while(0 != DeviceSRAMInit())
    {
        printf("SRAM Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**ADC初始化 */
    CoreADCInit();
    /**PWM DAC 初始化 */
    DevicePWM_DAC_Init();
    while(1)
    {
        DevicePWM_DAC_Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}