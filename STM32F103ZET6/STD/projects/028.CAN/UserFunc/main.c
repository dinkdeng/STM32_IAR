#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "DeviceSRAM.h"
#include "CoreCAN.h"

void ShowCanRecvPrintf(CanRxMsg* messagePtr)
{
    printf("rxMsg.IDE:  %08x\r\n", messagePtr->IDE);
    printf("rxMsg.RTR  %08x\r\n", messagePtr->RTR);
    printf("rxMsg.StdId  %08x\r\n", messagePtr->StdId);
    printf("rxMsg.ExtId  %08x\r\n", messagePtr->ExtId);
    printf("rxMsg.DLC  %08x\r\n", messagePtr->DLC);
    printf("rxMsg.Data : \r\n");
    for (uint8_t i = 0; i < messagePtr->DLC; i++)
    {
        printf("  %02x  ", messagePtr->Data[i]);
    }
    printf("\r\n");
}

void CanCallBackFunc(CanRxMsg* messagePtr)
{
    ShowCanRecvPrintf(messagePtr);
}

uint8_t canSendBuffer[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
uint32_t canStdId = 0x00F;
uint32_t canExtID = 0x0F0;

void SystemCAN_Test()
{
    if(DeviceExtiGetCount(EXTI_LEFT) > 0)
    {
        DeviceExtiClear(EXTI_LEFT);
        printf("Left Key Event\r\n");
        canStdId++;
        for(uint8_t i = 0; i < 8; i++)
            canSendBuffer[i]++;
        CoreCAN_SendMessage(CORE_CAN_MSG_STD,canStdId,canSendBuffer,8);
    }
    if(DeviceExtiGetCount(EXTI_RIGHT) > 0)
    {
        DeviceExtiClear(EXTI_RIGHT);
        printf("Right Key Event\r\n");
        canExtID++;
        for(uint8_t i = 0; i < 8; i++)
            canSendBuffer[i]++;
        CoreCAN_SendMessage(CORE_CAN_MSG_EXT,canExtID,canSendBuffer,8);
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
    /**CAN初始化*/
    CoreCAN_Init(CORE_CAN_500K,CanCallBackFunc);
    while(1)
    {
        SystemCAN_Test();
        recvLength = CoreSerialUart1DmaGetRecv(&uartRecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uartRecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();
        }
    }
}