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
#include "CoreRTC.h"


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

void RTC_Printf(CORE_RTC_TIME* rtcTime)
{

    printf("RTC Timer Is : year : %d  month : %d day : %d  week : %d \r\n", rtcTime->year,
        rtcTime->month, rtcTime->day, rtcTime->week);
    printf("               hour : %d  minute : %d  second : %d\r\n", rtcTime->hour,
        rtcTime->minute, rtcTime->second);
}



void DeviceDownExtiProcess()
{
    CORE_RTC_TIME rtcTime;
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    CoreRTC_GetTime(&rtcTime);
    RTC_Printf(&rtcTime);
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
    /**RTC初始化 */
    while(CoreRTC_Init(CORE_RTC_CLOCK_LSE))
    {
        printf("CoreRTC_Init Failed \r\n");
        CoreTickDelayMs(500);
    }
    /**配置WAKE UP中断,1秒钟中断一次 */
    CoreRTC_SetWakeup(RTC_WakeUpClock_CK_SPRE_16bits,0,WakeUpProc);
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(500);
    }
}




