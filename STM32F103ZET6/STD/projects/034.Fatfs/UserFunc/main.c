#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "DeviceSD.h"
#include "ff.h"

FATFS fs;
FRESULT fresult;
FIL fil;
uint32_t readCount = 0;

uint8_t fatfsReadBuffer[512] = {0};

/**显示Readme信息 */
void ShowReadmeMessage(void)
{
    fresult = f_open(&fil, "0:/readme.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if(fresult == FR_OK)
    {
        do
        {
            fresult = f_read(&fil,fatfsReadBuffer,512,&readCount);
            if(fresult == FR_OK)
            {
                CoreSerialUart1DmaSendBuffer(fatfsReadBuffer,readCount);
            }
            else
            {
                printf("f_read readme failed : %d\r\n",fresult);
            }
        }while(readCount == 512);
        f_close(&fil);
    }
    else
    {
        printf("f_open readme failed : %d\r\n",fresult);
    }
}

void SystemFatfs_Test()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        ShowReadmeMessage();
        DeviceExtiClear(EXTI_LEFT);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");

    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");
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
    /**文件系统加载 */
    do
    {
        fresult = f_mount(&fs,"0:/",1);
        if(fresult != FR_OK)
        {
            printf("f_mount Failed : %d\r\n",fresult);
            CoreTickDelayMs(500);
        }
    }while(fresult != FR_OK);
    while(1)
    {
        SystemFatfs_Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}