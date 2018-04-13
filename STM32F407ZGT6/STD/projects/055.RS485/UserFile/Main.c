#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceKey.h"
#include "DeviceBeep.h"
#include "CoreSerialUart1DMA.h"
#include "DeviceExti.h"
#include "DeviceLCD.h"
#include "CoreRand.h"
#include "DeviceRS485.h"


void LcdDrawLineRandom(void)
{
    DeviceLCD_Clear(BLACK);
    uint16_t startx = CoreRandGetNextByRange(0,480);
    uint16_t starty = CoreRandGetNextByRange(0,800);
    uint16_t endx = CoreRandGetNextByRange(0,480);
    uint16_t endy = CoreRandGetNextByRange(0,800);
    DeviceLCD_DrawRectangle(startx,starty,endx,endy,RED);
    DeviceLCD_ShowChar(0,0,'D');
    DeviceLCD_ShowString(0,0,"asdfghjklzxcvbnmqwertyuiopasdfghjkzxcvbnmqwertyuiop74185296312345678zxcvbnmasdfghjklqwertyuiop");
    DeviceLCD_ShowNum(0,500,123456,10);
}

void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    LcdDrawLineRandom();
}


void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    DeviceRS485_SendString("This Is RS485 Test\r\n");
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

void WakeUpProc()
{
    DeviceLedToogle(LED_INDEX_RED);
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
    CoreRandInit();
    /**LCD初始化 */
    while(DeviceLCD_Init())
    {
        printf("TFT_LCD Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    DeviceLCD_Clear(BLACK);
    DeviceRS485_Init(115200,DeviceRS485_DefaultCallBackFunc);
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(500);
    }
}




