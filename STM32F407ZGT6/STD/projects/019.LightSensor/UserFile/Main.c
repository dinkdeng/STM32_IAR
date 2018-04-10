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
#include "CoreChipTempSensor.h"
#include "DeviceLightSensor.h"


void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    printf("ADC Value : %d\r\n",CoreADC_GetValueBychannel(ADC_Channel_5,10));
    printf("Chip Temp Value : %0.2f\r\n",CoreChipTempSensorGet(10));
    printf("Light Sensor :%d \r\n",DeviceLightSensorGetPowerful(10));
}

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
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
}

int main(void)
{
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    /*初始化EXTI*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_DOWN);
    /**蜂鸣器初始化 */
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1DMA_Init(115200,CoreSerialUart1DMA_DefaultCallBack);
    /**ADC初始化*/
    CoreADC_Init();
    CoreChipTempSensorInit();
    DeviceLightSensorInit();
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(100);
    }
}




