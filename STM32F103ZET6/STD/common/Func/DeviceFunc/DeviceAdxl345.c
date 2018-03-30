#include "DeviceAdxl345.h"
#include "CoreTickDelay.h"
#include "CoreIIC.h"
#include <math.h>

/*启用错误信息调试使用的printf函数接口*/
#if (DEVICE_ADXL345_USE_PRINTF == 1)
#include <stdio.h>
#define DeviceAdxl345ShowError(format,args...)  printf(format,##args)
#else
#define DeviceAdxl345ShowError(format,args...)  ((void)0);
#endif


/*设备ID号*/
#define ADXL345_DEVICE_ID		0XE5

/*寄存器地址*/
/*器件ID寄存器*/
#define ADXL345_DEVICE_ID_REG	0X00

/*敲击阀值*/
#define THRESH_TAP		        0X1D
#define OFSX			        0X1E
#define OFSY			        0X1F
#define OFSZ			        0X20
#define DUR				        0X21
#define Latent			        0X22
#define Window  		        0X23
#define THRESH_ACK		        0X24
#define THRESH_INACT	        0X25
#define TIME_INACT		        0X26
#define ACT_INACT_CTL	        0X27
#define THRESH_FF		        0X28
#define TIME_FF			        0X29
#define TAP_AXES		        0X2A
#define ACT_TAP_STATUS          0X2B
#define BW_RATE			        0X2C
#define POWER_CTL		        0X2D

#define INT_ENABLE		        0X2E
#define INT_MAP			        0X2F
#define INT_SOURCE  	        0X30
#define DATA_FORMAT	            0X31
#define DATA_X0			        0X32
#define DATA_X1			        0X33
#define DATA_Y0			        0X34
#define DATA_Y1			        0X35
#define DATA_Z0			        0X36
#define DATA_Z1			        0X37
#define FIFO_CTL		        0X38
#define FIFO_STATUS		        0X39

/*如果ALT ADDRESS脚(12脚)接地,IIC地址为0X53(不包含最低位).*/
/*如果接V3.3,则IIC地址为0X1D(不包含最低位).*/
/*因为开发板接V3.3,所以转为读写地址后,为0X3B和0X3A(如果接GND,则为0XA7和0XA6)*/
#define ADXL_READ               0X3B
#define ADXL_WRITE              0X3A


static uint8_t DeviceAdxl345ReadRegSingle(uint8_t addr, uint8_t* regValue)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    /*发送起始讯号*/
    CoreIICSendStart();
    /*发送写器件指令*/
    CoreIICSendByte(ADXL_WRITE);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    /*发送寄存器地址*/
    CoreIICSendByte(addr);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    //重新启动
    CoreIICSendStart();
    //发送读器件指令
    CoreIICSendByte(ADXL_READ);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    *regValue = CoreIICRecvByte();
    CoreIICSendNACK();
    CoreIICSendStop();
    /*返回读到的值*/
    return 0;
}

static uint8_t DeviceAdxl345ReadRegMulti(uint8_t addr, uint8_t length, uint8_t* readValuePtr)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    /*发送起始讯号*/
    CoreIICSendStart();
    /*发送写器件指令*/
    CoreIICSendByte(ADXL_WRITE);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    /*发送寄存器地址*/
    CoreIICSendByte(addr);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    //重新启动
    CoreIICSendStart();
    //发送读器件指令
    CoreIICSendByte(ADXL_READ);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    for (uint8_t lengthIndex = 0; lengthIndex < length; lengthIndex++)
    {
        *(readValuePtr+lengthIndex) = CoreIICRecvByte();
        if (lengthIndex == length - 1)
        {
            CoreIICSendNACK();
        }
        else
        {
            CoreIICSendACK();
        }
    }
    CoreIICSendStop();
    /*返回读到的值*/
    return 0;
}


static uint8_t DeviceAdxl345WriteReg(uint8_t addr, uint8_t val)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    /*发送起始讯号*/
    CoreIICSendStart();
    /*发送写器件指令*/
    CoreIICSendByte(ADXL_WRITE);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    /*发送寄存器地址*/
    CoreIICSendByte(addr);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    /*发送想要写入的数据*/
    CoreIICSendByte(val);
    waitAckResult = CoreIICWaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        DeviceAdxl345ShowError("file=%s,func=%s,line=%d errCode = %x\r\n",
            __FILE__, __FUNCTION__, __LINE__, errCode);
        /*读取失败*/
        return 2;
    }
    CoreIICSendStop();
    /*返回读到的值*/
    return 0;
}



/*设备初始化*/
uint8_t DeviceAdxl345Init(void)
{
    uint8_t readID = 0;
    CoreIICInit();
    uint8_t iicResult = 0;
    iicResult = DeviceAdxl345ReadRegSingle(ADXL345_DEVICE_ID_REG, &readID);
    if (iicResult)
        return iicResult;
    //读取器件ID
    if (readID == ADXL345_DEVICE_ID)
    {
        /*低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 */
        DeviceAdxl345WriteReg(DATA_FORMAT, 0X2B);
        /*数据输出速度为100Hz*/
        DeviceAdxl345WriteReg(BW_RATE, 0x0A);
        /*链接使能,测量模式*/
        DeviceAdxl345WriteReg(POWER_CTL, 0x28);
        /*不使用中断*/
        DeviceAdxl345WriteReg(INT_ENABLE, 0x00);
        DeviceAdxl345WriteReg(OFSX, 0x00);
        DeviceAdxl345WriteReg(OFSY, 0x00);
        DeviceAdxl345WriteReg(OFSZ, 0x00);
        return 0;
    }
    return 3;
}

/*读取三轴数据*/
uint8_t DeviceAdxl345ReadXYZ(short *x, short *y, short *z,uint8_t times)
{
    uint8_t iicResult = 0;
    uint8_t buf[6];
    long tx = 0, ty = 0, tz = 0;
    for (uint8_t i = 0; i < times; i++)
    {
        iicResult = DeviceAdxl345ReadRegMulti(0x32, 6, buf);
        if(iicResult)
            return iicResult;
        tx += (short)(((uint16_t)buf[1] << 8) + buf[0]);
        ty += (short)(((uint16_t)buf[3] << 8) + buf[2]);
        tz += (short)(((uint16_t)buf[5] << 8) + buf[4]);
        CORE_IIC_WAIT_MS(5);
    }
    *x = (short)(tx / times);
    *y = (short)(ty / times);
    *z = (short)(tz / times);
    return 0;
}

/*读取角度*/
uint8_t DeviceAdxl345ReadAngle(float x, float y, float z,ADXL345_BASE_AXIS baseAxis, short* angle)
{
    float temp;
    float res = 0;
    switch (baseAxis)
    {
        /*与自然Z轴的角度*/
    case BASE_AXIS_Z:
        temp = sqrt((x*x + y*y)) / z;
        res = atan(temp);
        break;
        /*与自然X轴的角度*/
    case BASE_AXIS_X:
        temp = x / sqrt((y*y + z*z));
        res = atan(temp);
        break;
        /*与自然Y轴的角度*/
    case BASE_AXIS_Y:
        temp = y / sqrt((x*x + z*z));
        res = atan(temp);
        break;
    }
    /*返回计算值*/
    *angle = (short)((res * 1800) / 3.14);
    return 0;
}

/*自动校正*/
uint8_t DeviceAdxl345AutoAdjust(char *xval, char *yval, char *zval)
{
    short tx, ty, tz;
    uint8_t i;
    short offx = 0, offy = 0, offz = 0;
    //先进入休眠模式.
    DeviceAdxl345WriteReg(POWER_CTL, 0x00);
    CORE_IIC_WAIT_MS(100);
    //低电平中断输出,13位全分辨率,输出数据右对齐,16g量程
    DeviceAdxl345WriteReg(DATA_FORMAT, 0X2B);
    //数据输出速度为100Hz
    DeviceAdxl345WriteReg(BW_RATE, 0x0A);
    //链接使能,测量模式
    DeviceAdxl345WriteReg(POWER_CTL, 0x28);
    //不使用中断
    DeviceAdxl345WriteReg(INT_ENABLE, 0x00);
    DeviceAdxl345WriteReg(OFSX, 0x00);
    DeviceAdxl345WriteReg(OFSY, 0x00);
    DeviceAdxl345WriteReg(OFSZ, 0x00);
    CORE_IIC_WAIT_MS(12);
    uint8_t iicResult = 0;
    for (i = 0; i < 10; i++)
    {
        iicResult = DeviceAdxl345ReadXYZ(&tx, &ty, &tz, 10);
        /*读取失败*/
        if (iicResult)
            return iicResult;
        offx += tx;
        offy += ty;
        offz += tz;
    }
    /*10次读取完成,计算校准值*/
    offx /= 10;
    offy /= 10;
    offz /= 10;
    *xval = -offx / 4;
    *yval = -offy / 4;
    *zval = -(offz - 256) / 4;
    DeviceAdxl345WriteReg(OFSX, *xval);
    DeviceAdxl345WriteReg(OFSY, *yval);
    DeviceAdxl345WriteReg(OFSZ, *zval);

    return 0;
}


