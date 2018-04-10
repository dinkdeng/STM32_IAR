#include "DeviceMPU6050.h"
#include "CoreIIC.h"
#include "CoreTickDelay.h"

//通用MPU6050寄存器定义
//accel_offs寄存器,可读取版本号,寄存器手册未提到
//#define MPU_ACCEL_OFFS_REG		0X06
//prod id寄存器,在寄存器手册未提到
//#define MPU_PROD_ID_REG			0X0C
#define MPU_SELF_TESTX_REG		0X0D	//自检寄存器X
#define MPU_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU_SELF_TESTA_REG		0X10	//自检寄存器A
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU_CFG_REG				0X1A	//配置寄存器
#define MPU_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU_MOTION_DET_REG		0X1F	//运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG			0X23	//FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG		0X24	//IIC主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG	0X25	//IIC从机0器件地址寄存器
#define MPU_I2CSLV0_REG			0X26	//IIC从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG	0X27	//IIC从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG	0X28	//IIC从机1器件地址寄存器
#define MPU_I2CSLV1_REG			0X29	//IIC从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG	0X2A	//IIC从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG	0X2B	//IIC从机2器件地址寄存器
#define MPU_I2CSLV2_REG			0X2C	//IIC从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG	0X2D	//IIC从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG	0X2E	//IIC从机3器件地址寄存器
#define MPU_I2CSLV3_REG			0X2F	//IIC从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG	0X30	//IIC从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG	0X31	//IIC从机4器件地址寄存器
#define MPU_I2CSLV4_REG			0X32	//IIC从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG		0X33	//IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG	0X34	//IIC从机4控制寄存器
#define MPU_I2CSLV4_DI_REG		0X35	//IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG		0X36	//IIC主机状态寄存器
#define MPU_INTBP_CFG_REG		0X37	//中断/旁路设置寄存器
#define MPU_INT_EN_REG			0X38	//中断使能寄存器
#define MPU_INT_STA_REG			0X3A	//中断状态寄存器

#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG		0X3C	//加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG		0X3D	//加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG		0X3E	//加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG		0X3F	//加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG		0X40	//加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU_TEMP_OUTL_REG		0X42	//温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG		0X63	//IIC从机0数据寄存器
#define MPU_I2CSLV1_DO_REG		0X64	//IIC从机1数据寄存器
#define MPU_I2CSLV2_DO_REG		0X65	//IIC从机2数据寄存器
#define MPU_I2CSLV3_DO_REG		0X66	//IIC从机3数据寄存器

#define MPU_I2CMST_DELAY_REG	0X67	//IIC主机延时管理寄存器
#define MPU_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU_USER_CTRL_REG		0X6A	//用户控制寄存器
#define MPU_PWR_MGMT1_REG		0X6B	//电源管理寄存器1
#define MPU_PWR_MGMT2_REG		0X6C	//电源管理寄存器2
#define MPU_FIFO_CNTH_REG		0X72	//FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG		0X73	//FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG			0X74	//FIFO读写寄存器
#define MPU_DEVICE_ID_REG		0X75	//器件ID寄存器


//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//陀螺仪满量程范围
#define MPU6050_GYRO_FSR_250DPS		0
#define MPU6050_GYRO_FSR_500DPS		1
#define MPU6050_GYRO_FSR_1000DPS	2
#define MPU6050_GYRO_FSR_2000DPS	3
#define MPU6050_GYRO_DEFAULT_FSR	MPU6050_GYRO_FSR_2000DPS

//加速度计满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
#define MPU6050_ACCEL_FSR_2G	0
#define MPU6050_ACCEL_FSR_4G	1
#define MPU6050_ACCEL_FSR_8G	2
#define MPU6050_ACCEL_FSR_16G	3
#define MPU6050_ACCEL_DEFAULT_FSR	MPU6050_ACCEL_FSR_2G

//设置MPU6050的数字低通滤波器
#define MPU6050_DLPF_188_INFI	1
#define MPU6050_DLPF_98_188		2
#define MPU6050_DLPF_42_98		3
#define MPU6050_DLPF_20_42		4
#define MPU6050_DLPF_10_20		5
#define MPU6050_DLPF_0_10		6

//mpu6050的采样率 根据采样率自动设置低通滤波器为采样率的一半
//单位HZ
#define MPU6050_FS_MAX		1000
#define MPU6050_FS_MIN		4
#define MPU6050_DEFAULT_FS	100

/*启用错误信息调试使用的printf函数接口*/
#if (DEVICE_MPU6050_USE_PRINTF == 1)
#include <stdio.h>
#define DeviceMPU6050ShowError(format,args...)  printf(format,##args)
#else
#define DeviceMPU6050ShowError(format,args...)  ((void)0);
#endif

//设备地址
//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR				0X68


//IIC写一个字节
uint8_t DeviceMPU6050_WriteByte(uint8_t reg,uint8_t data)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    CoreIIC_SendStart();
	//发送器件地址+写命令
	CoreIIC_SendByte((MPU_ADDR<<1)|0);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    //写寄存器地址
    CoreIIC_SendByte(reg);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    //发送数据
	CoreIIC_SendByte(data);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
	CoreIIC_SendStop();
    return 0;
}

//IIC读一个字节
uint8_t DeviceMPU6050_ReadByte(uint8_t reg,uint8_t* readData)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    CoreIIC_SendStart();
	//发送器件地址+写命令
	CoreIIC_SendByte((MPU_ADDR<<1)|0);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    //写寄存器地址
    CoreIIC_SendByte(reg);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /**dummy Start */
    CoreIIC_SendStart();
	//发送器件地址+写命令
	CoreIIC_SendByte((MPU_ADDR<<1)|1);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /**接收一个字节的数据 */
    *readData = CoreIIC_RecvByte();
    CoreIIC_SendNACK();
    CoreIIC_SendStop();
    return 0;
}

//IIC连续写
//addr:器件地址
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//其他,错误代码
uint8_t DeviceMPU6050_WriteLen(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    CoreIIC_SendStart();
	//发送器件地址+写命令
	CoreIIC_SendByte((addr<<1)|0);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    //写寄存器地址
    CoreIIC_SendByte(reg);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
	for(uint8_t i=0;i<len;i++)
	{
		//发送数据
		CoreIIC_SendByte(buf[i]);
		/*等待设备响应*/
        waitAckResult = CoreIIC_WaitACK();
        if (IIC_WAIT_ACK_FAIL == waitAckResult)
        {
            /*显示调试错误讯息*/
            DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
            return 2;
        }
	}
    CoreIIC_SendStop();
    return 0;
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//其他,错误代码
uint8_t DeviceMPU6050_ReadLen(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    /**等待ACK的结果 */
    IIC_WAIT_ACK waitAckResult = IIC_WAIT_ACK_FAIL;
    CoreIIC_SendStart();
	//发送器件地址+写命令
	CoreIIC_SendByte((addr<<1)|0);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    //写寄存器地址
    CoreIIC_SendByte(reg);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
    /**dummy Start */
    CoreIIC_SendStart();
	//发送器件地址+写命令
	CoreIIC_SendByte((addr<<1)|1);
    /*等待设备响应*/
    waitAckResult = CoreIIC_WaitACK();
    if (IIC_WAIT_ACK_FAIL == waitAckResult)
    {
        /*显示调试错误讯息*/
        DeviceMPU6050ShowError("IIC WaitAck Error,file=%s,func=%s,line=%d\r\n",__FILE__, __FUNCTION__, __LINE__);
        return 2;
    }
	while(len)
	{
		if(len==1)
        {
            //读数据,发送nACK 只读一次
			*buf=CoreIIC_RecvByte();
            CoreIIC_SendNACK();
        }
		else
        {
            //读数据,发送ACK 连续读
			*buf=CoreIIC_RecvByte();
            CoreIIC_SendACK();
        }
		len--;
		buf++;
	}
	//产生一个停止条件
    CoreIIC_SendStop();
    return 0;
}


//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//MPU6050_GYRO_FSR_250DPS MPU6050_GYRO_FSR_500DPS
//MPU6050_GYRO_FSR_1000DPS MPU6050_GYRO_FSR_2000DPS
//返回值:0,设置成功
//    其他,设置失败
static uint8_t DeviceMPU6050_SetGyroFsr(uint8_t fsr)
{
    /**设置陀螺仪满量程范围  */
	return DeviceMPU6050_WriteByte(MPU_GYRO_CFG_REG,fsr<<3);
}


//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//MPU6050_ACCEL_FSR_2G MPU6050_ACCEL_FSR_4G
//MPU6050_ACCEL_FSR_8G MPU6050_ACCEL_FSR_16G
//返回值:0,设置成功
//    其他,设置失败
static uint8_t DeviceMPU6050_SetAccelFsr(uint8_t fsr)
{
	/**设置加速度传感器满量程范围  */
	return DeviceMPU6050_WriteByte(MPU_ACCEL_CFG_REG,fsr<<3);
}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败
static uint8_t DeviceMPU6050_SetLPF(uint16_t lpf)
{
	u8 data=0;
	if(lpf>=188)
		data=MPU6050_DLPF_188_INFI;
	else if(lpf>=98)
		data=MPU6050_DLPF_98_188;
	else if(lpf>=42)
		data=MPU6050_DLPF_42_98;
	else if(lpf>=20)
		data=MPU6050_DLPF_20_42;
	else if(lpf>=10)
		data=MPU6050_DLPF_10_20;
	else
		data=MPU6050_DLPF_0_10;
    /**设置数字低通滤波器 */
	return DeviceMPU6050_WriteByte(MPU_CFG_REG,data);
}

//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败
static uint8_t DeviceMPU6050_SetRate(uint16_t rate)
{
	u8 data;
	if(rate>MPU6050_FS_MAX)
		rate=MPU6050_FS_MAX;
	if(rate<MPU6050_FS_MIN)
		rate=MPU6050_FS_MIN;
	data=1000/rate-1;
    /**设置数字低通滤波器 */
	DeviceMPU6050_WriteByte(MPU_SAMPLE_RATE_REG,data);
    /**自动设置LPF为采样率的一半 */
 	return DeviceMPU6050_SetLPF(rate/2);
}




//得到温度值
//得到的温度是实际温度*100
uint8_t DeviceMPU6050_GetTemperature(short* temp)
{
    uint8_t buf[2];
    short raw;
	float tempVal;
	if(DeviceMPU6050_ReadLen(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf))
	{
		*temp = 0;
		//读取失败
		return 1;
	}
    raw=((u16)buf[0]<<8)|buf[1];
    tempVal=36.53+((double)raw)/340;
	*temp = (short)(tempVal*100);
    return 0;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t DeviceMPU6050_GetGyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6];
	if(DeviceMPU6050_ReadLen(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf))
	{
		*gx=0;
		*gy=0;
		*gz=0;
        return 1;
	}
	else
	{
        *gx=((u16)buf[0]<<8)|buf[1];
		*gy=((u16)buf[2]<<8)|buf[3];
		*gz=((u16)buf[4]<<8)|buf[5];
		return 0;
	}
}

//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t DeviceMPU6050_GetAccelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6];
	if(DeviceMPU6050_ReadLen(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf))
	{
		*ax=0;
		*ay=0;
		*az=0;
        return 1;
	}
	else
	{
        *ax=((u16)buf[0]<<8)|buf[1];
		*ay=((u16)buf[2]<<8)|buf[3];
		*az=((u16)buf[4]<<8)|buf[5];
        return 0;
	}
}

//初始化MPU6050
uint8_t DeviceMPU6050_Init(void)
{
	uint8_t DeviceID;
    uint8_t res;
	//初始化IIC总线
	CoreIIC_Init();
	//复位MPU6050
	res = DeviceMPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X80);
	if(res)return res;
	DEVICE_MPU6050_WAIT_MS(100);
	//唤醒MPU6050
	res = DeviceMPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X00);
	if(res)return res;
	DEVICE_MPU6050_WAIT_MS(10);
	//陀螺仪传感器,±2000dps
	res = DeviceMPU6050_SetGyroFsr(MPU6050_GYRO_DEFAULT_FSR);
	if(res)return res;
	//加速度传感器,±2g
	res = DeviceMPU6050_SetAccelFsr(MPU6050_ACCEL_DEFAULT_FSR);
	if(res)return res;
	//设置采样率50Hz
	res = DeviceMPU6050_SetRate(MPU6050_DEFAULT_FS);
	if(res)return res;
	//关闭所有中断
	res = DeviceMPU6050_WriteByte(MPU_INT_EN_REG,0X00);
	if(res)return res;
	//I2C主模式关闭;
	res = DeviceMPU6050_WriteByte(MPU_USER_CTRL_REG,0X00);
	if(res)return res;
	//关闭FIFO
	res = DeviceMPU6050_WriteByte(MPU_FIFO_EN_REG,0X00);
	if(res)return res;
	//INT引脚低电平有效
	res = DeviceMPU6050_WriteByte(MPU_INTBP_CFG_REG,0X80);
	if(res)return res;
	//读取设备ID号
	res = DeviceMPU6050_ReadByte(MPU_DEVICE_ID_REG,&DeviceID);
	if(res)return res;
	if(DeviceID==MPU_ADDR)//器件ID正确
	{
		//设置CLKSEL,PLL X轴为参考
		res = DeviceMPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X01);
		if(res)return res;
		//加速度与陀螺仪都工作
		res = DeviceMPU6050_WriteByte(MPU_PWR_MGMT2_REG,0X00);
		if(res)return res;
		//设置采样率为50Hz
		res = DeviceMPU6050_SetRate(MPU6050_DEFAULT_FS);
		return res;
	}
	else
		return 1;
}


