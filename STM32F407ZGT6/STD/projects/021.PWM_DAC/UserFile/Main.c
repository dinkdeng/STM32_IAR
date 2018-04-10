#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "CoreWWDG.h"
#include "CoreADC.h"
#include "DevicePWM_DAC.h"


void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    uint16_t adcValue = CoreADC_GetValueBychannel(ADC_Channel_5,10);
    printf("ADC Value : %d , Vol : %0.2f\r\n",adcValue,(adcValue*3.3)/4096);
}

uint16_t dacSetValue = 0;

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    dacSetValue -= 100;
    if(dacSetValue > 3300)
        dacSetValue = 0;
    DevicePWM_DAC_SetValue(dacSetValue);
    printf("DAC Set Value : %d , Vol : %0.2f \r\n",dacSetValue,dacSetValue/1000.0);
}

void DeviceUpExtiProcess()
{
    DeviceExtiClear(EXTI_UP);
    printf("Up Exti Press\r\n");
    dacSetValue += 100;
    if(dacSetValue > 3300)
        dacSetValue = 3300;
    DevicePWM_DAC_SetValue(dacSetValue);
    printf("DAC Set Value : %d , Vol : %0.2f \r\n",dacSetValue,dacSetValue/1000.0);
}

/**系统按键检测处理 */
void ExtiCheckLoop(void)
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceLeftExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceDownExtiProcess();
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceUpExtiProcess();
    }
}

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    /*初始化EXTI*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_DOWN);
    DeviceExtiInit(EXTI_UP);
    /**蜂鸣器初始化 */
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1DMA_Init(115200,CoreSerialUart1DMA_DefaultCallBack);
    /**ADC初始化*/
    CoreADC_Init();
    /**DAC初始化*/
    DevicePWM_DAC_Init();
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(100);
    }
}




