#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "DeviceSD.h"


uint8_t sdInfoBuffer[16] = {0};

uint8_t bufferSD[512] = {0};

void SystemSD_Test()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        /**获取SD卡类型 */
        printf("SD Card Type 0x%02x\r\n",SD_Type);
        uint32_t sectorCount = DeviceSD_GetSectorCount();
        printf("Sector Count %d , Capacity : %d MB\r\n",sectorCount,(sectorCount>>11));
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        DeviceSD_Get_CID_Info(sdInfoBuffer);
        printf("CID Info : \r\n");
        for(uint8_t i = 0; i < 16; i++)
        {
            printf("  0x%02x ",sdInfoBuffer[i]);
        }
        printf("\r\n");

        DeviceSD_Get_CSD_Info(sdInfoBuffer);
        printf("CSD Info : \r\n");
        for(uint8_t i = 0; i < 16; i++)
        {
            printf("  0x%02x ",sdInfoBuffer[i]);
        }
        printf("\r\n");
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");
        DeviceSD_ReadBlocks(bufferSD,0,1);
        printf("Read Sector 0 Data : \r\n");
        for(uint16_t i = 0; i < 512; i++)
        {
            printf("  0x%02x ",bufferSD[i]);
        }
        printf("\r\n");
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
    /**SD卡初始化 */
    while(DeviceSD_Init() != 0)
    {
        printf("DeviceSD_Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    while(1)
    {
        SystemSD_Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}