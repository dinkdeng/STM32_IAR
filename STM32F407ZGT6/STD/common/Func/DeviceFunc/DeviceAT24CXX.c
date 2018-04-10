#include "DeviceAT24CXX.h"
#include "CoreTickDelay.h"
#include "CoreIIC.h"

/*启用错误信息调试使用的printf函数接口*/
#if (DEVICE_AT24CXX_USE_PRINTF == 1)
#include <stdio.h>
#define DeviceAT24CxxShowError(format,args...)  printf(format,##args)
#else
#define DeviceAT24CxxShowError(format,args...)  ((void)0);
#endif


/*读取字节*/
static uint8_t DeviceAT24CxxReadByte(uint16_t addr, uint8_t* readDat)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    /**地址校验 */
    if (addr > DEVICE_SIZE_AT24CXX_USED)
        return 1;
    /*发送启动指令*/
    CoreIIC_SendStart();
    if (DEVICE_SIZE_AT24CXX_USED > DEVICE_SIZE_AT24C16)
    {
        /*写入设备写地址*/
        CoreIIC_SendByte(DEVICE_AT24CXX_ADDR_W);
        /*等待设备响应*/
        waitAckResult = CoreIIC_WaitACK();
        if (IIC_WAIT_ACK_FAIL == waitAckResult)
        {
            /*显示调试错误讯息*/
            DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
            return 2;
        }
        /*写入目标地址高地址*/
        CoreIIC_SendByte((uint8_t)(addr >> 8));
    }
    else
    {
        /*发送器件地址,写数据*/
        CoreIIC_SendByte(DEVICE_AT24CXX_ADDR_W + (uint8_t)((addr / 256) << 1));
    }
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",
            __FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /*写入目标地址高地址*/
    CoreIIC_SendByte((uint8_t)(addr % 256));
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",
            __FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /*发送启动指令*/
    CoreIIC_SendStart();
    /*写入设备读地址*/
    CoreIIC_SendByte(DEVICE_AT24CXX_ADDR_R);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",
            __FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /*读取一个字节*/
    *readDat = CoreIIC_RecvByte();
    /*发送NACK*/
    CoreIIC_SendNACK();
    /*产生一个停止条件*/
    CoreIIC_SendStop();
    return 0;
}


/*写入字节*/
static uint8_t DeviceAT24CxxWriteByte(uint16_t addr, uint8_t writeDat)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    /**地址校验 */
    if (addr > DEVICE_SIZE_AT24CXX_USED)
        return 1;
    /*发送启动指令*/
    CoreIIC_SendStart();
    if (DEVICE_SIZE_AT24CXX_USED > DEVICE_SIZE_AT24C16)
    {
        /*写入设备写地址*/
        CoreIIC_SendByte(DEVICE_AT24CXX_ADDR_W);
        /*等待设备响应*/
        waitAckResult = CoreIIC_WaitACK();
        if (IIC_WAIT_ACK_FAIL == waitAckResult)
        {
            /*显示调试错误讯息*/
            DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
            return 2;
        }
        /*写入目标地址高地址*/
        CoreIIC_SendByte((uint8_t)(addr >> 8));
    }
    else
    {
        /*发送器件地址,写数据*/
        CoreIIC_SendByte(DEVICE_AT24CXX_ADDR_W + (uint8_t)((addr / 256) << 1));
    }
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",
            __FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /*写入目标地址高地址*/
    CoreIIC_SendByte((uint8_t)(addr % 256));
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",
            __FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /*发送字节*/
    CoreIIC_SendByte(writeDat);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceAT24CxxShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",
            __FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /*产生一个停止条件*/
    CoreIIC_SendStop();
    CORE_IIC_WAIT_US(10000);
    return 0;
}

/**设备初始化 */
uint8_t DeviceAT24CxxInit(void)
{
    uint8_t temp1;
    CoreIIC_Init();
    /*读取指定地址*/
    return DeviceAT24CxxReadByte(DEVICE_SIZE_AT24CXX_USED, &temp1);
}

/**读取数据块 */
uint8_t DeviceAT24CxxReadBuffer(uint16_t address,uint16_t length,uint8_t* readBufferPtr)
{
    if ((address + length-1) > DEVICE_SIZE_AT24CXX_USED)
        return 1;
    uint16_t index = 0;
    uint8_t readResult = 0;
    for (index = 0; index < length; index++)
    {
        readResult = DeviceAT24CxxReadByte(address + index, readBufferPtr + index);
        if (0 != readResult)
            break;
    }
    return readResult;
}

/**写入数据块 */
uint8_t DeviceAT24CxxWriteBuffer(uint16_t address,uint16_t length,uint8_t* writeBufferPtr)
{
    if ((address + length-1) > DEVICE_SIZE_AT24CXX_USED)
        return 1;
    uint16_t index = 0;
    uint8_t readResult = 0;
    for (index = 0; index < length; index++)
    {
        readResult = DeviceAT24CxxWriteByte(address + index, *(writeBufferPtr + index));
        if (0 != readResult)
            break;
    }
    return readResult;
}