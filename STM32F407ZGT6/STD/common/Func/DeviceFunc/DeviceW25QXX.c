#include "DeviceW25QXX.h"
#include "SystemUtil.h"
#include "CoreSPI.h"
#include "CoreTickDelay.h"


/**状态寄存器定义
 * 状态寄存器
 * bit0 芯片忙标志(1,忙;0,空闲)
 * bit1 :写使能锁定
 * bit2 3 4 共同构成扇区区域写保护 都为0时没有保护
 * bit5 顶部与底部块区保护
 * bit6 保留
 * bit7 srp 状态寄存器保护 为1可以写状态寄存器 为0 不能写
 */

/**指令定义 */
#define DEVICE_W25QXX_COMMAND_WRITE_ENABLE						0x06    //写使能
#define DEVICE_W25QXX_COMMAND_WRITE_DISABLE					    0x04    //写禁止
#define DEVICE_W25QXX_COMMAND_READ_STATUS_REG				    0x05    //读状态寄存器
#define DEVICE_W25QXX_COMMAND_WRITE_STATUS_REG			        0x01    //写状态寄存器
#define DEVICE_W25QXX_COMMAND_READ_DATA						    0x03    //读数据
#define DEVICE_W25QXX_COMMAND_PAGE_PROGRAM					    0x02    //页编程
#define DEVICE_W25QXX_COMMAND_BLOCK_ERASE						0xD8    //块擦除
#define DEVICE_W25QXX_COMMAND_SECTOR_ERASE					    0x20    //扇区擦除
#define DEVICE_W25QXX_COMMAND_CHIP_ERASE						0xC7    //片擦除
#define DEVICE_W25QXX_COMMAND_POWER_DOWN						0xB9    //关机
#define DEVICE_W25QXX_COMMAND_POWER_ON							0xAB    //释放关机
#define DEVICE_W25QXX_COMMAND_DEVICE_ID						    0xAB    //读取设备ID
#define DEVICE_W25QXX_COMMAND_MANUFACT_DEVICEID			        0x90    //读取制造ID
#define DEVICE_W25QXX_COMMAND_JEDE_CDEVICE_ID				    0x9F    //读取兼容性ID


/** 初始化SPI FLASH的IO口*/
static void DeviceW25QXX_IOInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//使能GPIOG时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//GPIOB14 PB14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	//输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//100MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//初始化
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PG7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	//初始化
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	//PG7输出1,防止NRF干扰SPI FLASH的通信
	GPIO_SetBits(GPIOG,GPIO_Pin_7);

    DeviceW25QXXSetCS(1);

    /**初始化SPI */
    CoreSPI1_Init(DEVICE_W25QXX_CPOL,DEVICE_W25QXX_CPHA,DEVICE_W25QXX_SPEED);
}

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t DeviceW25QXXReadSR(void)
{
    u8 result=0;
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    /**发送读取状态寄存器命令 */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_READ_STATUS_REG);
    /**读取一个字节*/
    result=CoreSPI1_WriteRead(0Xff);
    /**取消片选 */
    DeviceW25QXXSetCS(1);
    return result;
}

//写SPI_FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void DeviceW25QXXWriteSR(uint8_t statusRegVal)
{
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    /**发送写取状态寄存器命令  */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_WRITE_STATUS_REG);
    /**写入一个字节 */
    CoreSPI1_WriteRead(statusRegVal);
    /**取消片选 */
    DeviceW25QXXSetCS(1);
}

//SPI_FLASH写使能
//将WEL置位
static void DeviceW25QXXWriteEnable(void)
{
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    /**发送写使能  */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_WRITE_ENABLE);
    /**取消片选 */
    DeviceW25QXXSetCS(1);
}

//SPI_FLASH写禁止
//将WEL清零
void DeviceW25QXXWriteDisable(void)
{
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    /**发送写禁止  */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_WRITE_DISABLE);
    /**取消片选 */
    DeviceW25QXXSetCS(1);
}

//读取芯片ID W25X16的ID:0XEF14
static uint16_t DeviceW25QXXReadID(void)
{
    uint16_t temp = 0;
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    /**发送读取ID命令*/
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_MANUFACT_DEVICEID);
    CoreSPI1_WriteRead(0x00);
    CoreSPI1_WriteRead(0x00);
    CoreSPI1_WriteRead(0x00);
    temp += CoreSPI1_WriteRead(0xFF);
    temp <<= 8;
    temp += CoreSPI1_WriteRead(0xFF);
    /**取消片选 */
    DeviceW25QXXSetCS(1);
    return temp;
}

//flash检测 检测到返回0 失败返回1
static uint8_t DeviceW25QXXCheck(void)
{
    uint16_t flashId = 0;
    flashId = DeviceW25QXXReadID();
    if(flashId == DEVICE_W25QXX_ID)
        return 0;
    else
        return 1;
}

//等待空闲
static void DeviceW25QXXWaitBusy(void)
{
    uint8_t readVal;
    /**等待BUSY位清空 */
    do
    {
        readVal = DeviceW25QXXReadSR();
    }while((readVal&0x01)==0x01);
}


//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//写入数据长度不应该超过该页的剩余字节数!!!
static uint8_t DeviceW25QXXWritePage(uint8_t* pBuffer,uint32_t writeAddr,uint16_t numByteToWrite)
{
    uint16_t i;
    /**超出地址限制 */
    if(writeAddr+numByteToWrite >= DEVICE_W25QXX_ADDR_MAX)
        return 1;
    /**SET WEL  */
    DeviceW25QXXWriteEnable();
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    /**发送写页命令 */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_PAGE_PROGRAM);
    /**发送24bit地址 */
    CoreSPI1_WriteRead((uint8_t)((writeAddr)>>16));
    CoreSPI1_WriteRead((uint8_t)((writeAddr)>>8));
    CoreSPI1_WriteRead((uint8_t)writeAddr);
    for(i=0;i<numByteToWrite;i++)
    {
        /**循环写数  */
        CoreSPI1_WriteRead(pBuffer[i]);
    }
    /**取消片选 */
    DeviceW25QXXSetCS(1);
    /**等待写入结束 */
    DeviceW25QXXWaitBusy();
    return 0;
}


//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
static uint8_t DeviceW25QXXWriteNoCheck(uint8_t* pBuffer,uint32_t writeAddr,uint16_t numByteToWrite)
{
    uint16_t pageremain;
     /**超出地址限制 */
    if(writeAddr+numByteToWrite >= DEVICE_W25QXX_ADDR_MAX)
        return 1;
    /**单页剩余的字节数 */
    pageremain=DEVICE_W25QXX_PAGE_SIZE-writeAddr%DEVICE_W25QXX_PAGE_SIZE;
    /**不大于256个字节*/
    if(numByteToWrite<=pageremain)
        pageremain=numByteToWrite;
    while(1)
    {
        DeviceW25QXXWritePage(pBuffer,writeAddr,pageremain);
        /**写入结束了 */
        if(numByteToWrite==pageremain)
            break;
        else //numByteToWrite>pageremain
        {
            /**地址增长 */
            pBuffer+=pageremain;
            writeAddr+=pageremain;
            /**减去已经写入了的字节数 */
            numByteToWrite-=pageremain;
            /**一次可以写入256个字节*/
            if(numByteToWrite>DEVICE_W25QXX_PAGE_SIZE)
                pageremain=DEVICE_W25QXX_PAGE_SIZE;
            /**不够256个字节了 */
            else
                pageremain=numByteToWrite;
        }
    };
    return 0;
}

//擦除整个芯片
//整片擦除时间:
//W25X16:25s
//W25X32:40s
//W25X64:40s
//等待时间超长...
void DeviceW25QXXEraseChip(void)
{
    /**SET WEL  */
    DeviceW25QXXWriteEnable();
    /**等待写入结束 */
    DeviceW25QXXWaitBusy();
    /**使能器件  */
    DeviceW25QXXSetCS(0);
    /**发送片擦除命令  */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_CHIP_ERASE);
    /**取消片选     */
    DeviceW25QXXSetCS(1);
    /**等待芯片擦除结束*/
    DeviceW25QXXWaitBusy();
}

//擦除一个扇区
//Dst_Addr:扇区地址 0~511 for w25x16
//擦除一个山区的最少时间:150ms
void DeviceW25QXXEraseSector(uint32_t dstAddr)
{
    dstAddr*=(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM);
    /**SET WEL  */
    DeviceW25QXXWriteEnable();
    DeviceW25QXXWaitBusy();
    /**使能器件  */
    DeviceW25QXXSetCS(0);
    /**发送扇区擦除指令  */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_SECTOR_ERASE);
    /**发送24bit地址  */
    CoreSPI1_WriteRead((u8)((dstAddr)>>16));
    CoreSPI1_WriteRead((u8)((dstAddr)>>8));
    CoreSPI1_WriteRead((u8)dstAddr);
    /**取消片选  */
    DeviceW25QXXSetCS(1);
    /**等待擦除完成*/
    DeviceW25QXXWaitBusy();
}

//进入掉电模式
void DeviceW25QXXPowerDown(void)
{
    /**使能器件    */
    DeviceW25QXXSetCS(0);
    /**发送掉电命令 */
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_POWER_DOWN);
    /**取消片选  */
    DeviceW25QXXSetCS(1);
    /**等待TPD */
    CoreTickDelayMs(3);
}

//唤醒
void DeviceW25QXXWakeUp(void)
{
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_POWER_ON);
    DeviceW25QXXSetCS(1);
    CoreTickDelayMs(3);
}


/**芯片初始化 */
uint8_t DeviceW25QXX_Init(void)
{
    DeviceW25QXX_IOInit();
    return DeviceW25QXXCheck();
}

//读取SPI FLASH
//在指定地址开始读取指定长度的数据
uint8_t DeviceW25QXX_Read(uint8_t* pBuffer,uint32_t readAddr,uint16_t numByteToRead)
{
    uint16_t i;
    /**超出地址限制 */
    if(readAddr+numByteToRead >= DEVICE_W25QXX_ADDR_MAX)
        return 1;
    /**设置SPI接口到系统指定模式 */
    CoreSPI1_Set_CPXX(DEVICE_W25QXX_CPOL,DEVICE_W25QXX_CPHA);
    CoreSPI1_SetSpeed(DEVICE_W25QXX_SPEED);
    /**使能器件 */
    DeviceW25QXXSetCS(0);
    CoreSPI1_WriteRead(DEVICE_W25QXX_COMMAND_READ_DATA);
    /**发送24bit地址  */
    CoreSPI1_WriteRead((uint8_t)((readAddr)>>16));
    CoreSPI1_WriteRead((uint8_t)((readAddr)>>8));
    CoreSPI1_WriteRead((uint8_t)readAddr);
    for(i=0;i<numByteToRead;i++)
    {
        pBuffer[i] = CoreSPI1_WriteRead(0XFF);
    }
    /**取消片选 */
    DeviceW25QXXSetCS(1);
    return 0;
}

//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
uint8_t deviceW25QXXBuffer[DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM];

uint8_t DeviceW25QXX_Write(uint8_t* pBuffer,uint32_t writeAddr,uint16_t numByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    /**超出地址限制 */
    if(writeAddr+numByteToWrite >= DEVICE_W25QXX_ADDR_MAX)
        return 1;
    /**扇区地址 */
    secpos=writeAddr/(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM);
    /**在扇区内的偏移 */
    secoff=writeAddr%(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM);
    /**扇区剩余空间大小  */
    secremain=(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM)-secoff;

    /**设置为设备的特定SPI状态 */
    CoreSPI1_Set_CPXX(DEVICE_W25QXX_CPOL,DEVICE_W25QXX_CPHA);
    CoreSPI1_SetSpeed(DEVICE_W25QXX_SPEED);

    /**不大于4096个字节 */
    if(numByteToWrite<=secremain)
        secremain=numByteToWrite;
    while(1)
    {
        /**读出整个扇区的内容 */
        DeviceW25QXX_Read(deviceW25QXXBuffer,secpos*(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM),DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM);
        /**校验数据,初始化擦除之后是0XFF */
        for(i=0;i<secremain;i++)
        {
            /**需要擦除  */
            if(deviceW25QXXBuffer[secoff+i]!=0XFF)
                break;
        }
        if(i<secremain)//需要擦除
        {
            /**擦除这个扇区 */
            DeviceW25QXXEraseSector(secpos);
            for(i=0;i<secremain;i++)	   //复制
            {
                deviceW25QXXBuffer[i+secoff]=pBuffer[i];
            }
            /**写入整个扇区 */
            DeviceW25QXXWriteNoCheck(deviceW25QXXBuffer,secpos*(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM),(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM));

        }
        /**写已经擦除了的,直接写入扇区剩余区间.  */
        else
            DeviceW25QXXWriteNoCheck(pBuffer,writeAddr,secremain);
        /**写入结束了*/
        if(numByteToWrite==secremain)
            break;
        /**写入未结束 */
        else
        {
            /**扇区地址增1 */
            secpos++;
            /**偏移位置为0 */
            secoff=0;
            /**指针偏移 */
            pBuffer+=secremain;
            /**写地址偏移 */
            writeAddr+=secremain;
            /**字节数递减 */
            numByteToWrite-=secremain;
            /**下一个扇区还是写不完*/
            if(numByteToWrite>(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM))
                secremain=(DEVICE_W25QXX_PAGE_SIZE*DEVICE_W25QXX_PAGE_NUM);
            /**下一个扇区可以写完了 */
            else
                secremain=numByteToWrite;
        }
    };
    return 0;
}











