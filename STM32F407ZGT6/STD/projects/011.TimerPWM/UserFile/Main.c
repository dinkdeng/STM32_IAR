#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "CoreWWDG.h"
#include "CoreTimerPWM.h"

uint32_t dutySet = 1;

void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    dutySet += 10;
    if(dutySet > 999)
        dutySet = 999;
    CoreTimer14PWM_SetDuty(dutySet);
    printf("Left Exti Press\r\n");
}

void DeviceRightExtiProcess()
{
    DeviceExtiClear(EXTI_RIGHT);
    printf("Right Exti Press\r\n");
}

void DeviceUpExtiProcess()
{
    DeviceExtiClear(EXTI_UP);
    printf("Up Exti Press\r\n");
}

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    dutySet -= 10;
    if(dutySet > 999)
        dutySet = 1;
    CoreTimer14PWM_SetDuty(dutySet);
    printf("Down Exti Press\r\n");
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
    SystemUtilSetIntGroup();
    CoreTickDelayInit();
    DeviceLedInit(LED_STATUS_OFF);
    /*初始化EXTI*/
    DeviceExtiInit(EXTI_LEFT);
    DeviceExtiInit(EXTI_UP);
    DeviceExtiInit(EXTI_DOWN);
    /**蜂鸣器初始化 */
    DeviceBeepInit(BEEP_OFF);
    CoreSerialUart1DMA_Init(115200,CoreSerialUart1DMA_DefaultCallBack);
    /**PWM初始化 1K主频*/
    CoreTimer14PWM_Init(999,83);
    CoreTimer14PWM_Start(dutySet);
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(500);
    }
}




