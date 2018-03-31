#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "CoreWWDG.h"
#include "CoreSTM32Flash.h"

/*设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)*/
#define USER_STM_TEST_ADDR     0X08070000

uint16_t flashTestBuffer[10];

void ShowFlashTestBuffer()
{
    uint8_t i = 0;
    printf("Flash Buffer: \t");
    for(i = 0; i < 10; i++)
    {
        printf("0X%02X  ",flashTestBuffer[i]);
    }
    printf("\r\n");
}

void IncFlashBuffer()
{
    uint8_t i = 0;
    for(i = 0; i < 10; i++)
    {
        flashTestBuffer[i]++;
    }
}

void ClearFlashBuffer()
{
    for(uint8_t i = 0; i < 10; i++)
    {
        flashTestBuffer[i] = 0;
    }
}

void STM32FlashTest()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        ClearFlashBuffer();
        CoreSTM32FlashRead(USER_STM_TEST_ADDR,flashTestBuffer,10);
        printf("STM32 Flash Read Over\r\n");
        ShowFlashTestBuffer();
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        IncFlashBuffer();
        CoreSTM32FlashWrite(USER_STM_TEST_ADDR,flashTestBuffer,10);
        printf("STM32 Flash Write Over\r\n");
        ShowFlashTestBuffer();
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
    /*初始化Led*/
    DeviceLedInit(LED_STATUS_OFF);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);
    while(1)
    {
        STM32FlashTest();
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