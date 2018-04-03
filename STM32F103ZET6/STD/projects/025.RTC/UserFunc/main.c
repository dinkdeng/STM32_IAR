#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "MemManagerLocal.h"
#include "MemManagerSRAM.h"
#include "DeviceW25qxx.h"
#include "DeviceNRF24L01.h"

uint8_t nrf24l01Buffer[32];


void NRF24L01Test()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        DeviceNRF24L01ModeChangeTX();
        printf("Change Mode TX\r\n");
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        DeviceNRF24L01ModeChangeRX();
        printf("Change Mode RX\r\n");
    }
    if(DeviceExtiGetCount(EXTI_UP) > 0)
    {
        DeviceExtiClear(EXTI_UP);
        printf("Up Key Event\r\n");
        for(uint8_t i = 0; i < 32; i++)
            nrf24l01Buffer[i] = i;
        if(0 != DeviceNRF24L01SendPackage(nrf24l01Buffer))
        {
            printf("DeviceNRF24L01SendPackage Failed\r\n");
        }
        else
        {
            printf("DeviceNRF24L01SendPackage Success,0-31\r\n");
        }
    }
    if(DeviceExtiGetCount(EXTI_DOWN) > 0)
    {
        DeviceExtiClear(EXTI_DOWN);
        printf("Down Key Event\r\n");
        if(0 == DeviceNRF24L01RecvPackage(nrf24l01Buffer))
        {
            printf("Recv NRF24L01 Data : ");
            for(int j = 0; j < 32; j++)
            {
                printf("  0x%02X",nrf24l01Buffer[j]);
            }
            printf("\r\n");
        }
        else
        {
            printf("NRF24L01 No Data Recv\r\n");
        }
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
    MemManagerLocalInit();
    MemManagerSRAMInit();
    /**初始化W25QXX */
    while(0 != DeviceW25QXXInit())
    {
        printf("DeviceW25QXX Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    /**初始化NRF24L01 */
    while(0 != DeviceNRF24L01Init())
    {
        printf("DeviceNRF24L01Init Init Failed\r\n");
        CoreTickDelayMs(500);
    }
    while(1)
    {
        NRF24L01Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}