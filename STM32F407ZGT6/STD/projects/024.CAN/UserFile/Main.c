#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "CoreRand.h"
#include "CoreCAN.h"


uint8_t canSendBuffer[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
uint32_t canStdId = 0x00F;
uint32_t canExtID = 0x0F0;

void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    canStdId++;
    for (uint8_t i = 0; i < 8; i++)
        canSendBuffer[i]++;
    CoreCAN_SendMessage(CORE_CAN_MSG_STD, canStdId, canSendBuffer, 8);
}

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    canExtID++;
    for (uint8_t i = 0; i < 8; i++)
        canSendBuffer[i]++;
    CoreCAN_SendMessage(CORE_CAN_MSG_EXT, canExtID, canSendBuffer, 8);
}

void DeviceUpExtiProcess()
{
    DeviceExtiClear(EXTI_UP);
    printf("Up Exti Press\r\n");
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
    CoreRandInit();
    CoreCAN_Init(CORE_CAN_500K,ShowCanRecvPrintf);
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(1000);
    }
}




