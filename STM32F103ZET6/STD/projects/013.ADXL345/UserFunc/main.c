#include "stm32f10x.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"
#include "DeviceLed.h"
#include "DeviceExti.h"
#include "CoreSerialUart1Dma.h"
#include "DeviceBeep.h"
#include "CoreWWDG.h"
#include "DeviceAdxl345.h"

int main(void)
{
    uint8_t* uart1RecvDatBufferPtr = NULL;
    uint16_t recvLength = 0;

    uint8_t readResult = 0;
    short xValue, yValue, zValue;
    short angle;

    /*初始化系统分组*/
    SystemUtilSetIntGroup();
    /*初始化延时函数*/
    CoreTickDelayInit();
    /*初始化Led*/
    DeviceLedInit(LED_STATUS_OFF);
    /**初始化串口 */
    CoreSerialUart1DmaInit(115200);
    /**每次复位自动打印该数据 */
    CoreSerialUart1DmaSendString("SystemInit\r\n");
    /**初始化Beep */
    DeviceBeepInit(BEEP_OFF);

    /**初始化ADXL345 */
    while(DeviceAdxl345Init())
    {
        printf("DeviceAdxl345Init Filed\r\n");
        CoreTickDelayMs(1000);
    }

    while(1)
    {
        /*读取数据和角度*/
        readResult = DeviceAdxl345ReadXYZ(&xValue, &yValue, &zValue, 10);
        if (readResult)
        {
            printf("DeviceAdxl345ReadXYZ ErrCode : %x \r\n", readResult);
        }
        else
        {
            DeviceAdxl345ReadAngle((float)xValue, (float)yValue, (float)zValue,BASE_AXIS_Z, &angle);
            printf("xaxis : %d , yaxis : %d , zaxis : %d , angle : %f \r\n", xValue,yValue, zValue, angle / 1.0);
        }
        recvLength = CoreSerialUart1DmaGetRecv(&uart1RecvDatBufferPtr);
        if(recvLength != 0)
        {
            CoreSerialUart1DmaSendBuffer(uart1RecvDatBufferPtr,recvLength);
            CoreSerialUart1DmaSendString("\r\n");
            CoreSerialUart1DmaClearRx();

            readResult = DeviceAdxl345AutoAdjust((char*)&xValue, (char*)&yValue,(char*)&zValue);
            if (readResult != 0)
            {
                printf("DeviceAdxl345AutoAdjust ErrCode : %x \r\n", readResult);
            }
            printf("DeviceAdxl345AutoAdjust \r\n");
        }
        CoreTickDelayMs(1000);
    }
}