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
#include "DeviceSDIO.h"


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


//通过串口打印SD卡相关信息
void ShowSDIO_Info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:
			printf("Card Type:SDSC V1.1\r\n");
			break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:
			printf("Card Type:SDSC V2.0\r\n");
			break;
		case SDIO_HIGH_CAPACITY_SD_CARD:
			printf("Card Type:SDHC V2.0\r\n");
			break;
		case SDIO_MULTIMEDIA_CARD:
			printf("Card Type:MMC Card\r\n");
			break;
	}
    /**制造商ID */
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);
    /**卡相对地址 */
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);
    /**显示容量 */
	printf("Card Capacity:%d MB\r\n",(uint32_t)(SDCardInfo.CardCapacity>>20));
    /**显示块大小*/
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);
}

void DeviceLeftExtiProcess()
{
    DeviceExtiClear(EXTI_LEFT);
    printf("Left Exti Press\r\n");
    LcdDrawLineRandom();
}

uint8_t sdioBuffer[1024] = {0};

void DeviceDownExtiProcess()
{
    DeviceExtiClear(EXTI_DOWN);
    printf("Down Exti Press\r\n");
    ShowSDIO_Info();
    for(uint32_t sectorAddr = 0; sectorAddr < 1000; sectorAddr+=2)
    {
        if (DeviceSDIO_ReadDisk(sdioBuffer, sectorAddr, 2) == 0)
        {
            printf("*");
        }
        else
        {
            printf("!");
            break;
        }
        if(DeviceSDIO_WriteDisk(sdioBuffer,sectorAddr,2) == 0)
        {
            printf("-");
        }
        else
        {
            printf("!");
            break;
        }
    }
    printf("\r\nTest Over\r\n");
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
    CoreRandInit();
    /**LCD初始化 */
    while(DeviceLCD_Init())
    {
        printf("TFT_LCD Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    DeviceLCD_Clear(BLACK);
    /**SD卡初始化 */
    while(DeviceSDIO_Init())
    {
        printf("Device SDIO Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    printf("System Init Over\r\n");
    while(1)
    {
        ExtiCheckLoop();
        CoreTickDelayMs(500);
    }
}




