#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "CoreWWDG.h"
#include "CoreTimerCapture.h"


void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    double maxTimer = 0.0;
    uint32_t captureCount = 0;
    captureCount = CoreTimer5CaptureGetCountWithTime(&maxTimer);
    printf("captureCount : %d   maxTimer = %0.1f \r\n",captureCount,maxTimer);
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
    /**输入捕获初始化 */
    CoreTimer5CaptureInit();
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(500);
    }
}




