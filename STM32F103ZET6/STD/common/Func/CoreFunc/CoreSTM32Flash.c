#include "CoreSTM32Flash.h"

/*最多是2K字节,缓冲区*/
static uint16_t coreStm32FlashBufferForSector[CORE_STM32_FLASH_SECTOR_SIZE / 2];

/**读取指定地址的半字(16位数据)
 *  faddr:读地址(此地址必须为2的倍数!!)
 *  返回值:对应数据. */
uint16_t CoreSTM32FlashReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr;
}

//不检查的写入 没有擦除过程
//writeAddr:起始地址
//pBuffer:数据指针
//numToWrite:半字(16位)数   
void CoreSTM32FlashWriteNoCheck(u32 writeAddr, u16 *pBuffer, u16 numToWrite)
{
    uint16_t i;
    for (i = 0; i < numToWrite; i++)
    {
        FLASH_ProgramHalfWord(writeAddr, pBuffer[i]);
        //地址增加2.16位数据
        writeAddr += 2;
    }
}


//从指定地址开始写入指定长度的数据,带有自动擦除功能
//writeAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//numToWrite:半字(16位)数(就是要写入的16位数据的个数.)
uint8_t CoreSTM32FlashWrite(uint32_t writeAddr,uint16_t *pBuffer, uint16_t numToWrite)
{
    //扇区地址
    uint32_t secpos;
    //扇区内偏移地址(16位字计算)
    uint16_t secoff;
    //扇区内剩余地址(16位字计算)	
    uint16_t secremain;
    uint16_t i;
    //去掉0X08000000后的地址
    uint32_t offaddr;
    //非法地址
    if (writeAddr < CORE_STM32_FLASH_BASE || (writeAddr >= (CORE_STM32_FLASH_BASE + 1024 * CORE_STM32_FLASH_KB_SIZE)))
        return 1;
    /*写入之前要判定HSI是否打开,没打开要打开*/
    /*0 HSI初始化未启动, 1 HSI初始化已经启动*/
    uint8_t hsiFlag = 0;
    if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
        hsiFlag = 0;
    else
        hsiFlag = 1;
    /**打开HSI*/
    if (hsiFlag == 0)
    {
        /*操作FLASH 写入和擦除的时候必须打开HSI*/
        RCC_HSICmd(ENABLE);
        /*等待HSI稳定*/
        while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
    }

    //解锁
    FLASH_Unlock();
    //实际偏移地址.
    offaddr = writeAddr - CORE_STM32_FLASH_BASE;
    //扇区地址  0~127 for STM32F103RBT6
    secpos = offaddr / CORE_STM32_FLASH_SECTOR_SIZE;
    //在扇区内的偏移(2个字节为基本单位.)
    secoff = (offaddr%CORE_STM32_FLASH_SECTOR_SIZE) / 2;
    //扇区剩余空间大小
    secremain = CORE_STM32_FLASH_SECTOR_SIZE / 2 - secoff;
    //不大于该扇区范围
    if (numToWrite <= secremain)
        secremain = numToWrite;
    while (1)
    {
        //读出整个扇区的内容到缓冲区中,擦除以后不会丢失数据
        CoreSTM32FlashRead(secpos*CORE_STM32_FLASH_SECTOR_SIZE + CORE_STM32_FLASH_BASE, coreStm32FlashBufferForSector, CORE_STM32_FLASH_SECTOR_SIZE / 2);
        //校验数据
        for (i = 0; i < secremain; i++)
        {
            //需要擦除  	
            if (coreStm32FlashBufferForSector[secoff + i] != 0XFFFF)
                break;
        }
        //需要擦除
        if (i < secremain)
        {
            //擦除这个扇区
            FLASH_ErasePage(secpos*CORE_STM32_FLASH_SECTOR_SIZE + CORE_STM32_FLASH_BASE);
            //复制
            for (i = 0; i < secremain; i++)
            {
                coreStm32FlashBufferForSector[i + secoff] = pBuffer[i];
            }
            //写入整个扇区  
            CoreSTM32FlashWriteNoCheck(secpos*CORE_STM32_FLASH_SECTOR_SIZE + CORE_STM32_FLASH_BASE, coreStm32FlashBufferForSector, CORE_STM32_FLASH_SECTOR_SIZE / 2);
        }
        //写已经擦除了的,直接写入扇区剩余区间.
        else
            CoreSTM32FlashWriteNoCheck(writeAddr, pBuffer, secremain);
        //写入结束了
        if (numToWrite == secremain)
            break;
        //写入未结束
        else
        {
            //扇区地址增1
            secpos++;
            //偏移位置为0
            secoff = 0;
            //指针偏移
            pBuffer += secremain;
            //写地址偏移
            writeAddr += secremain;
            //字节(16位)数递减
            numToWrite -= secremain;
            //下一个扇区还是写不完
            if (numToWrite > (CORE_STM32_FLASH_SECTOR_SIZE / 2))
                secremain = CORE_STM32_FLASH_SECTOR_SIZE / 2;
            //下一个扇区可以写完了
            else
                secremain = numToWrite;
        }
    }
    //上锁
    FLASH_Lock();
    if (hsiFlag == 0)
    {
        /*根据之前的状态选择关闭还是打开HSI*/
        RCC_HSICmd(DISABLE);
    }
    return 0;
}

/** 从指定地址开始读出指定长度的数据 
 *      readAddr:起始地址 
 *      pBuffer:数据指针 
 *      numToWrite:半字(16位)数.
 */

uint8_t CoreSTM32FlashRead(uint32_t readAddr,uint16_t *pBuffer, uint16_t numToRead)
{
    u16 i;
    /*非法地址*/
    if (readAddr < CORE_STM32_FLASH_BASE || (readAddr >= (CORE_STM32_FLASH_BASE + 1024 * CORE_STM32_FLASH_KB_SIZE)))
        return 1;
    for (i = 0; i < numToRead; i++)
    {
        //读取2个字节.
        pBuffer[i] = CoreSTM32FlashReadHalfWord(readAddr);
        //偏移2个字节.
        readAddr += 2;
    }
    return 0;
}


