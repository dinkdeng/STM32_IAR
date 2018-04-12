#include "DeviceSDIO.h"

//SDIO相关标志位,拷贝自:stm32f4xx_sdio.h
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001)
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002)
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004)
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008)
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010)
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020)
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040)
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080)
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100)
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200)
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400)
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800)
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000)
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000)
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000)
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000)
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000)
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000)
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000)
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000)
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000)
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000)
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000)
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000)

////////////////////////////////////////////////////////////////////////////////////////////////////
//SDIO 指令集
#define SD_CMD_GO_IDLE_STATE                       ((u8)0)
#define SD_CMD_SEND_OP_COND                        ((u8)1)
#define SD_CMD_ALL_SEND_CID                        ((u8)2)
#define SD_CMD_SET_REL_ADDR                        ((u8)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((u8)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((u8)5)
#define SD_CMD_HS_SWITCH                           ((u8)6)
#define SD_CMD_SEL_DESEL_CARD                      ((u8)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((u8)8)
#define SD_CMD_SEND_CSD                            ((u8)9)
#define SD_CMD_SEND_CID                            ((u8)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((u8)11) /*!< SD Card doesn't support it */
#define SD_CMD_STOP_TRANSMISSION                   ((u8)12)
#define SD_CMD_SEND_STATUS                         ((u8)13)
#define SD_CMD_HS_BUSTEST_READ                     ((u8)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((u8)15)
#define SD_CMD_SET_BLOCKLEN                        ((u8)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((u8)17)
#define SD_CMD_READ_MULT_BLOCK                     ((u8)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((u8)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((u8)20)
#define SD_CMD_SET_BLOCK_COUNT                     ((u8)23)
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((u8)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((u8)25)
#define SD_CMD_PROG_CID                            ((u8)26)
#define SD_CMD_PROG_CSD                            ((u8)27)
#define SD_CMD_SET_WRITE_PROT                      ((u8)28)
#define SD_CMD_CLR_WRITE_PROT                      ((u8)29)
#define SD_CMD_SEND_WRITE_PROT                     ((u8)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((u8)32) /*!< To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SD_CMD_SD_ERASE_GRP_END                    ((u8)33) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SD_CMD_ERASE_GRP_START                     ((u8)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define SD_CMD_ERASE_GRP_END                       ((u8)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define SD_CMD_ERASE                               ((u8)38)
#define SD_CMD_FAST_IO                             ((u8)39) /*!< SD Card doesn't support it */
#define SD_CMD_GO_IRQ_STATE                        ((u8)40) /*!< SD Card doesn't support it */
#define SD_CMD_LOCK_UNLOCK                         ((u8)42)
#define SD_CMD_APP_CMD                             ((u8)55)
#define SD_CMD_GEN_CMD                             ((u8)56)
#define SD_CMD_NO_CMD                              ((u8)64)

/**
  * @brief Following commands are SD Card Specific commands.
  *        SDIO_APP_CMD ：CMD55 should be sent before sending these commands.
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((u8)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STAUS                        ((u8)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((u8)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((u8)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((u8)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((u8)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((u8)53) /*!< For SD I/O Card only */

/**
  * @brief Following commands are SD Card Specific security commands.
  *        SDIO_APP_CMD should be sent before sending these commands.
  */
#define SD_CMD_SD_APP_GET_MKB                      ((u8)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((u8)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((u8)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((u8)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((u8)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((u8)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((u8)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((u8)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((u8)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((u8)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((u8)48) /*!< For SD Card only */


//SDIO相关参数定义
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00010000)
#define SDIO_DATATIMEOUT                ((u32)0xFFFFFFFF)
#define SDIO_FIFO_Address               ((u32)0x40018080)

//Mask for errors Card Status R1 (OCR Register)
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CC_ERROR                 ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)

//Masks for R6 Response
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((u32)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0xFFFFFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF)

#define SD_HALFFIFO                     ((u32)0x00000008)
#define SD_HALFFIFOBYTES                ((u32)0x00000020)

//Command Class Supported
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)

//CMD8指令
#define SDIO_SEND_IF_COND               ((u32)0x00000008)
///////////////////////////////////////////////////////////////////////////////////////////

/*用于sdio初始化的结构体*/
SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;


/**SD卡类型（默认为1.x卡） */
static uint8_t CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;

/**SD卡CSD,CID以及相对地址(RCA)数据 */
static uint32_t CSD_Tab[4],CID_Tab[4],RCA=0;

/**SD卡信息*/
SD_CardInfo SDCardInfo;

/**辅助指针,保证指针的四字节对其特性,防止编译器的优化 */
#pragma pack(4)
uint32_t *tempbuff;
#pragma pack()

//SD_ReadDisk/SD_WriteDisk函数专用buf,当这两个函数的数据缓存区地址不是4字节对齐的时候,
//需要用到该数组,确保数据缓存区地址是4字节对齐的.
#pragma pack(4)
u8 SDIO_DATA_BUFFER[512];
#pragma pack()


/**寄存器初始化 */
void DeviceSDIO_RegDeInit()
{
    SDIO->POWER = 0x00000000;
    SDIO->CLKCR = 0x00000000;
    SDIO->ARG = 0x00000000;
    SDIO->CMD = 0x00000000;
    SDIO->DTIMER = 0x00000000;
    SDIO->DLEN = 0x00000000;
    SDIO->DCTRL = 0x00000000;
    SDIO->ICR = 0x00C007FF;
    SDIO->MASK = 0x00000000;
}

//SDIO时钟初始化设置
//clkdiv:时钟分频系数
//CK时钟=SDIOCLK/[clkdiv+2];(SDIOCLK时钟固定为48Mhz)
void DeviceSDIO_ClockSet(u8 clkdiv)
{
    uint32_t tmpreg = SDIO->CLKCR;
    tmpreg &= 0XFFFFFF00;
    tmpreg |= clkdiv;
    SDIO->CLKCR = tmpreg;
}


//检查CMD0的执行状态
//返回值:sd卡错误码
SD_Error DeviceSDIO_Cmd0Check(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t timeout = SDIO_CMD0TIMEOUT;
    while (timeout--)
    {
        /**命令已发送(无需响应) */
        if (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)
            break;
    }
    if (timeout == 0)
        return SD_CMD_RSP_TIMEOUT;
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return errorstatus;
}



//检查R7响应的错误状态
//返回值:sd卡错误码
SD_Error DeviceSDIO_CheckCmdResp7(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t timeout = SDIO_CMD0TIMEOUT;
    while (timeout--)
    {
        status = SDIO->STA;
        /**CRC错误/命令响应超时/已经收到响应(CRC校验成功) */
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
            break;
    }
    /**响应超时 */
    if ((timeout == 0) || (status & (1 << 2)))
    {
        /**当前卡不是2.0兼容卡,或者不支持设定的电压范围*/
        errorstatus = SD_CMD_RSP_TIMEOUT;
        /**清除命令响应超时标志 */
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return errorstatus;
    }
    /**成功接收到响应 */
    if (status & 1 << 6)
    {
        errorstatus = SD_OK;
        /**清除响应标志 */
        SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    }
    return errorstatus;
}

//检查R1响应的错误状态
//cmd:当前命令
//返回值:sd卡错误码
SD_Error DeviceSDIO_CheckCmdResp1(uint8_t cmd)
{
    uint32_t status;
    while (1)
    {
        status = SDIO->STA;
        /**CRC错误/命令响应超时/已经收到响应(CRC校验成功) */
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
            break;
    }
    /**响应超时 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        /**清除命令响应超时标志*/
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    /**CRC错误 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        /**清除标志*/
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_CMD_CRC_FAIL;
    }
    /**命令不匹配  */
    if (SDIO->RESPCMD != cmd)
        return SD_ILLEGAL_CMD;
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    /**返回卡响应 */
    return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS);
}

//检查R3响应的错误状态
//返回值:错误状态
SD_Error DeviceSDIO_CheckCmdResp3(void)
{
    uint32_t status;
    while (1)
    {
        status = SDIO->STA;
        /**CRC错误/命令响应超时/已经收到响应(CRC校验成功) */
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
            break;
    }
    /**响应超时 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        /**清除命令响应超时标志 */
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return SD_OK;
}

//检查R2响应的错误状态
//返回值:错误状态
SD_Error DeviceSDIO_CheckCmdResp2(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t timeout = SDIO_CMD0TIMEOUT;
    while (timeout--)
    {
        status = SDIO->STA;
        /**CRC错误/命令响应超时/已经收到响应(CRC校验成功)*/
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
            break;
    }
    /**响应超时 */
    if ((timeout == 0) || (status & (1 << 2)))
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        /**清除命令响应超时标志 */
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return errorstatus;
    }
    /**CRC错误 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        errorstatus = SD_CMD_CRC_FAIL;
        /**清除响应标志 */
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    }
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return errorstatus;
}

//检查R6响应的错误状态
//cmd:之前发送的命令
//prca:卡返回的RCA地址
//返回值:错误状态
SD_Error DeviceSDIO_CheckCmdResp6(uint8_t cmd, uint16_t *prca)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t rspr1;
    while (1)
    {
        status = SDIO->STA;
        /**CRC错误/命令响应超时/已经收到响应(CRC校验成功)*/
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
            break;
    }
    /**响应超时 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        /**清除命令响应超时标志*/
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    /**CRC错误 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        /**清除响应标志 */
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_CMD_CRC_FAIL;
    }
    /**判断是否响应cmd命令 */
    if (SDIO->RESPCMD != cmd)
    {
        return SD_ILLEGAL_CMD;
    }
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    /**得到响应  */
    rspr1 = SDIO->RESP1;
    if (SD_ALLZERO == (rspr1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
    {
        /**右移16位得到,rca */
        *prca = (uint16_t)(rspr1 >> 16);
        return errorstatus;
    }
    if (rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR)
        return SD_GENERAL_UNKNOWN_ERROR;
    if (rspr1 & SD_R6_ILLEGAL_CMD)
        return SD_ILLEGAL_CMD;
    if (rspr1 & SD_R6_COM_CRC_FAILED)
        return SD_COM_CRC_FAILED;
    return errorstatus;
}

/**SDIO发送指令 arg 参数  wait 是否等待中断*/
void DeviceSDIO_SendCommand(uint32_t arg,uint32_t cmdIndex,uint32_t response,uint32_t wait,uint32_t cpsmEnable)
{
     /**发送CMD0进入IDLE STAGE模式命令. */
     SDIO_CmdInitStructure.SDIO_Argument = arg;
     /**cmd0 */
     SDIO_CmdInitStructure.SDIO_CmdIndex = cmdIndex;
     /**无响应 */
     SDIO_CmdInitStructure.SDIO_Response = response;
     /**是否等待中断或挂起 */
     SDIO_CmdInitStructure.SDIO_Wait = wait;
     /**则CPSM在开始发送命令之前等待数据传输结束。 */
     SDIO_CmdInitStructure.SDIO_CPSM = cpsmEnable;
     /**写命令进命令寄存器 */
     SDIO_SendCommand(&SDIO_CmdInitStructure);
}

//卡上电,上电过程中会获取到卡类型
//查询所有SDIO接口上的卡设备,并查询其电压和配置时钟
//返回值:错误代码;(0,无错误)
SD_Error DeviceSDIO_PowerON(void)
{
    uint8_t i = 0;
    SD_Error errorstatus = SD_OK;
    uint32_t response = 0, count = 0, validvoltage = 0;
    uint32_t SDType = SD_STD_CAPACITY;

    /*初始化时的时钟不能大于400KHz*/
    /* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    /**不使用bypass模式，直接用HCLK进行分频得到SDIO_CK */
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    /**空闲时不关闭时钟电源 */
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    /**1位数据线 */
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    /**硬件流 */
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    //SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Enable;
    SDIO_Init(&SDIO_InitStructure);

    /**上电状态,开启卡时钟 */
    SDIO_SetPowerState(SDIO_PowerState_ON);
    /**SDIOCK使能 */
    SDIO->CLKCR |= 1 << 8;

    for (i = 0; i < 74; i++)
    {
        /** 发送CMD0进入IDLE STAGE模式命令 无响应,CPSM在开始发送命令之前等待数据传输结束*/
        DeviceSDIO_SendCommand(0x0,SD_CMD_GO_IDLE_STATE,SDIO_Response_No,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**检查CMD0的执行状态 */
        errorstatus = DeviceSDIO_Cmd0Check();
        /**获取到响应,退出 */
        if (errorstatus == SD_OK)
            break;
    }
    /**返回错误状态 */
    if (errorstatus)
        return errorstatus;

    /**发送CMD8,短响应,检查SD卡接口特性,cmd8,r7,关闭等待中断 */
    DeviceSDIO_SendCommand(SD_CHECK_PATTERN,SDIO_SEND_IF_COND,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    /**等待R7响应 */
    errorstatus = DeviceSDIO_CheckCmdResp7();
    /**R7响应正常 */
    if (errorstatus == SD_OK)
    {
        /**SD 2.0卡 */
        CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
        /**高容量卡 */
        SDType = SD_HIGH_CAPACITY;
    }

    /**发送CMD55,短响应 */
    DeviceSDIO_SendCommand(0x00,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    /**等待R1响应 */
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_APP_CMD);

    /**SD2.0/SD 1.1,否则为MMC卡 */
    if (errorstatus == SD_OK)
    {
        /**设置卡片电压 */
        //SD卡,发送ACMD41 SD_APP_OP_COND,参数为:0x80100000
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            /**发送CMD55,短响应 */
            DeviceSDIO_SendCommand(0x00,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            /**等待R1响应 */
            errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_APP_CMD);
            /**响应错误 */
            if (errorstatus != SD_OK)
                return errorstatus;

            /*acmd41，命令参数由支持的电压范围及HCS位组成，HCS位置一来区分卡是SDSc还是sdhc*/
            /**发送ACMD41,短响应 r3*/
            DeviceSDIO_SendCommand(SD_VOLTAGE_WINDOW_SD | SDType,SD_CMD_SD_APP_OP_COND,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            /**等待R3响应 */
            errorstatus = DeviceSDIO_CheckCmdResp3();
            /**响应错误 */
            if (errorstatus != SD_OK)
                return errorstatus;

            /**得到响应 */
            response = SDIO->RESP1;
            /**判断SD卡上电是否完成*/
            validvoltage = (((response >> 31) == 1) ? 1 : 0);
            count++;
        }
        if (count >= SD_MAX_VOLT_TRIAL)
        {
            /**电压不支持 */
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        if (response &= SD_HIGH_CAPACITY)
        {
            CardType = SDIO_HIGH_CAPACITY_SD_CARD;
        }
    }
    else
    {
        /**MMC卡 */
        //MMC卡,发送CMD1 SDIO_SEND_OP_COND,参数为:0x80FF8000
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            /**发送CMD1,短响应 r3*/
            DeviceSDIO_SendCommand(SD_VOLTAGE_WINDOW_MMC,SD_CMD_SEND_OP_COND,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            /**等待R3响应 */
            errorstatus = DeviceSDIO_CheckCmdResp3();
            /**响应错误 */
            if (errorstatus != SD_OK)
                return errorstatus;

            /**得到响应 */
            response = SDIO->RESP1;
            validvoltage = (((response >> 31) == 1) ? 1 : 0);
            count++;
        }
        if (count >= SD_MAX_VOLT_TRIAL)
        {
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        CardType = SDIO_MULTIMEDIA_CARD;
    }
    return errorstatus;
}

//SD卡 Power OFF
//返回值:错误代码;(0,无错误)
SD_Error DeviceSDIO_PowerOFF(void)
{
    /**SDIO电源关闭,时钟停止 */
    SDIO_SetPowerState(SDIO_PowerState_OFF);
    return SD_OK;
}

//初始化所有的卡,并让卡进入就绪状态
//返回值:错误代码,改API在卡上电之后调用
//并获取了CID和CSD信息
SD_Error DeviceSDIO_InitializeCards(void)
{
    SD_Error errorstatus = SD_OK;
    /**驱动第一张卡0x01 */
    uint16_t rca = 0x01;

    /**检查电源状态,确保为上电状态 */
    if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
    {
        errorstatus = SD_REQUEST_NOT_APPLICABLE;
        return (errorstatus);
    }
    /**非SECURE_DIGITAL_IO_CARD ,该函数在Power之后执行,此时就已经知道卡的类型了*/
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
    {
        /**发送CMD2,取得CID,长响应 */
        DeviceSDIO_SendCommand(0x0,SD_CMD_ALL_SEND_CID,SDIO_Response_Long,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R2响应 */
        errorstatus = DeviceSDIO_CheckCmdResp2();
        /**响应错误 */
        if (errorstatus != SD_OK)
            return errorstatus;

        /**读取CID信息 */
        CID_Tab[0] = SDIO->RESP1;
        CID_Tab[1] = SDIO->RESP2;
        CID_Tab[2] = SDIO->RESP3;
        CID_Tab[3] = SDIO->RESP4;
    }
    /**判断卡类型 */
    if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
        /** 发送CMD3,短响应,r6*/
        DeviceSDIO_SendCommand(0x00,SD_CMD_SET_REL_ADDR,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R6响应 */
        errorstatus = DeviceSDIO_CheckCmdResp6(SD_CMD_SET_REL_ADDR, &rca);
        /**响应错误 */
        if (errorstatus != SD_OK)
            return errorstatus;
    }
    /**MMC卡 */
    if (SDIO_MULTIMEDIA_CARD == CardType)
    {
        /**发送CMD3,短响应 r6*/
        DeviceSDIO_SendCommand((uint32_t)(rca << 16),SD_CMD_SET_REL_ADDR,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R2响应 */
        errorstatus = DeviceSDIO_CheckCmdResp2();
        /**响应错误 */
        if (errorstatus != SD_OK)
            return errorstatus;
    }
    /**非SECURE_DIGITAL_IO_CARD */
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
    {
        RCA = rca;
        /**发送CMD9+卡RCA,取得CSD,长响应 */
        DeviceSDIO_SendCommand((uint32_t)(rca << 16),SD_CMD_SEND_CSD,SDIO_Response_Long,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R2响应 */
        errorstatus = DeviceSDIO_CheckCmdResp2();
        /**响应错误 */
        if (errorstatus != SD_OK)
            return errorstatus;
        /**读取CSD信息 */
        CSD_Tab[0] = SDIO->RESP1;
        CSD_Tab[1] = SDIO->RESP2;
        CSD_Tab[2] = SDIO->RESP3;
        CSD_Tab[3] = SDIO->RESP4;
    }
    /**卡初始化成功 */
    return SD_OK;
}


//得到卡信息
//cardinfo:卡信息存储区
//返回值:错误状态
/**解析初始化的时候接收到的CSD和CID信息 */
SD_Error DeviceSDIO_GetCardInfo(SD_CardInfo *cardinfo)
{
    SD_Error errorstatus = SD_OK;
    uint8_t tmp = 0;
    cardinfo->CardType = (uint8_t)CardType; //卡类型
    cardinfo->RCA = (uint16_t)RCA;          //卡RCA值
    tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;        //CSD结构
    cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;   //2.0协议还没定义这部分(为保留),应该是后续协议定义的
    cardinfo->SD_csd.Reserved1 = tmp & 0x03;               //2个保留位
    tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);           //第1个字节
    cardinfo->SD_csd.TAAC = tmp;                           //数据读时间1
    tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);            //第2个字节
    cardinfo->SD_csd.NSAC = tmp;                           //数据读时间2
    tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);                   //第3个字节
    cardinfo->SD_csd.MaxBusClkFrec = tmp;                  //传输速度
    tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);           //第4个字节
    cardinfo->SD_csd.CardComdClasses = tmp << 4;           //卡指令类高四位
    tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);           //第5个字节
    cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4; //卡指令类低四位
    cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;              //最大读取数据长度
    tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);            //第6个字节
    cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;    //允许分块读
    cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;  //写块错位
    cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;  //读块错位
    cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.Reserved2 = 0;                                                                                                         //保留
    if ((CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0) || (SDIO_MULTIMEDIA_CARD == CardType)) //标准1.1/2.0卡/MMC卡
    {
        cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10; //C_SIZE(12位)
        tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);              //第7个字节
        cardinfo->SD_csd.DeviceSize |= (tmp) << 2;
        tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24); //第8个字节
        cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;
        cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
        cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);
        tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16); //第9个字节
        cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
        cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
        cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1; //C_SIZE_MULT
        tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);         //第10个字节
        cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
        cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1); //计算卡容量
        cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
        cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen); //块大小
        cardinfo->CardCapacity *= cardinfo->CardBlockSize;
    }
    else if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) //高容量卡
    {
        tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);              //第7个字节
        cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16; //C_SIZE
        tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);      //第8个字节
        cardinfo->SD_csd.DeviceSize |= (tmp << 8);
        tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16); //第9个字节
        cardinfo->SD_csd.DeviceSize |= (tmp);
        tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);                                         //第10个字节
        cardinfo->CardCapacity = (long long)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024; //计算卡容量
        cardinfo->CardBlockSize = 512;                                                      //块大小固定为512字节
    }
    cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;
    tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF); //第11个字节
    cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
    cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);
    tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24); //第12个字节
    cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
    cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;
    tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16); //第13个字节
    cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
    cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.Reserved3 = 0;
    cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);
    tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8); //第14个字节
    cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
    cardinfo->SD_csd.ECC = (tmp & 0x03);
    tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF); //第15个字节
    cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
    cardinfo->SD_csd.Reserved4 = 1;
    tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24); //第0个字节
    cardinfo->SD_cid.ManufacturerID = tmp;
    tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16); //第1个字节
    cardinfo->SD_cid.OEM_AppliID = tmp << 8;
    tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8); //第2个字节
    cardinfo->SD_cid.OEM_AppliID |= tmp;
    tmp = (uint8_t)(CID_Tab[0] & 0x000000FF); //第3个字节
    cardinfo->SD_cid.ProdName1 = tmp << 24;
    tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24); //第4个字节
    cardinfo->SD_cid.ProdName1 |= tmp << 16;
    tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16); //第5个字节
    cardinfo->SD_cid.ProdName1 |= tmp << 8;
    tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8); //第6个字节
    cardinfo->SD_cid.ProdName1 |= tmp;
    tmp = (uint8_t)(CID_Tab[1] & 0x000000FF); //第7个字节
    cardinfo->SD_cid.ProdName2 = tmp;
    tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24); //第8个字节
    cardinfo->SD_cid.ProdRev = tmp;
    tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16); //第9个字节
    cardinfo->SD_cid.ProdSN = tmp << 24;
    tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8); //第10个字节
    cardinfo->SD_cid.ProdSN |= tmp << 16;
    tmp = (uint8_t)(CID_Tab[2] & 0x000000FF); //第11个字节
    cardinfo->SD_cid.ProdSN |= tmp << 8;
    tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24); //第12个字节
    cardinfo->SD_cid.ProdSN |= tmp;
    tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16); //第13个字节
    cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
    cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;
    tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8); //第14个字节
    cardinfo->SD_cid.ManufactDate |= tmp;
    tmp = (uint8_t)(CID_Tab[3] & 0x000000FF); //第15个字节
    cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
    cardinfo->SD_cid.Reserved2 = 1;
    return errorstatus;
}

//选卡
//发送CMD7,选择相对地址(rca)为addr的卡,取消其他卡.如果为0,则都不选择.
//addr:卡的RCA地址
SD_Error DeviceSDIO_SelectDeselect(u32 addr)
{
    /**发送CMD7,选择卡,短响应 */
    DeviceSDIO_SendCommand(addr,SD_CMD_SEL_DESEL_CARD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    return DeviceSDIO_CheckCmdResp1(SD_CMD_SEL_DESEL_CARD);
}

SD_Error DeviceSDIO_TransErrorProcess(void)
{
    /**数据超时错误 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        /**清错误标志 */
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        return SD_DATA_TIMEOUT;
    }
    /**数据块CRC错误 */
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        /**清错误标志 */
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        return SD_DATA_CRC_FAIL;
    }
    /**接收fifo上溢错误 */
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        /**清错误标志 */
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        return SD_RX_OVERRUN;
    }
    /**接收起始位错误 */
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        /**清错误标志 */
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        return SD_START_BIT_ERR;
    }
    /**发送fifo下溢错误 */
    else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
        return SD_TX_UNDERRUN;
    }
    return SD_OK;
}


//查找SD卡的SCR寄存器值
//rca:卡相对地址
//pscr:数据缓存区(存储SCR内容)
//返回值:错误状态
SD_Error DeviceSDIO_FindSCR(uint16_t rca,uint32_t *pscr)
{
    uint32_t index = 0;
    SD_Error errorstatus = SD_OK;
    uint32_t tempscr[2] = {0, 0};

    /**发送CMD16,短响应,设置Block Size为8字节 cmd16 r1*/
    DeviceSDIO_SendCommand((uint32_t)8,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SET_BLOCKLEN);
    if (errorstatus != SD_OK)
        return errorstatus;

    /**发送CMD55,短响应 */
    DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_APP_CMD);
    if (errorstatus != SD_OK)
        return errorstatus;

    /**设置SDIO数据长度 */
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = 8;
    /**块大小8byte */
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /**发送ACMD51,短响应,参数为0 r1 */
    DeviceSDIO_SendCommand(0x0,SD_CMD_SD_APP_SEND_SCR,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SD_APP_SEND_SCR);
    if (errorstatus != SD_OK)
        return errorstatus;

    while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
        /**接收FIFO数据可用 */
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
        {
            /**读取FIFO内容 */
            *(tempscr + index) = SDIO->FIFO;
            index++;
            if (index >= 2)
                break;
        }
    }
    errorstatus = DeviceSDIO_TransErrorProcess();
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    if(errorstatus)
        return errorstatus;
    //把数据顺序按8位为单位倒过来.
    *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);
    *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);
    return errorstatus;
}

//SDIO使能宽总线模式
//enx:0,不使能;1,使能;
//返回值:错误状态
SD_Error DeviceSDIO_EnableWideBusLowLevel(uint8_t enx)
{
    SD_Error errorstatus = SD_OK;
    uint32_t scr[2] = {0, 0};
    uint8_t arg = 0X00;
    if (enx)
        arg = 0X02;
    else
        arg = 0X00;
    /**检查卡是否处于锁定 */
    if (SDIO->RESP1 & SD_CARD_LOCKED)
        return SD_LOCK_UNLOCK_FAILED;
    /**得到SCR寄存器数据 */
    errorstatus = DeviceSDIO_FindSCR(RCA, scr);
    if (errorstatus != SD_OK)
        return errorstatus;

    /**支持宽总线 */
    if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
    {
        /**发送CMD55+RCA,短响应 */
        DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_APP_CMD);
        if (errorstatus != SD_OK)
            return errorstatus;

        /**发送ACMD6,短响应,参数:10,4位;00,1位. */
        DeviceSDIO_SendCommand(arg,SD_CMD_APP_SD_SET_BUSWIDTH,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_APP_SD_SET_BUSWIDTH);
        return errorstatus;
    }
    /**不支持宽总线设置 */
    else
        return SD_REQUEST_NOT_APPLICABLE;
}

//设置SDIO总线宽度(MMC卡不支持4bit模式)
//wmode:位宽模式.0,1位数据宽度;1,4位数据宽度;2,8位数据宽度
//返回值:SD卡错误状态
//设置SDIO总线宽度(MMC卡不支持4bit模式)
//   @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
//   @arg SDIO_BusWide_4b: 4-bit data transfer
//   @arg SDIO_BusWide_1b: 1-bit data transfer (默认)
//返回值:SD卡错误状态
SD_Error DeviceSDIO_EnableWideBus(uint32_t WideMode)
{
    SD_Error errorstatus = SD_OK;
    /**根据卡片类型决定是否能切换 */
    if (SDIO_MULTIMEDIA_CARD == CardType)
    {
        errorstatus = SD_UNSUPPORTED_FEATURE;
        return (errorstatus);
    }
    else if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
        /**2.0 sd不支持8bits */
        if (SDIO_BusWide_8b == WideMode)
        {
            errorstatus = SD_UNSUPPORTED_FEATURE;
            return (errorstatus);
        }
        else
        {
            errorstatus = DeviceSDIO_EnableWideBusLowLevel(WideMode);
            if (SD_OK == errorstatus)
            {
                /**清除之前的位宽设置 */
                SDIO->CLKCR &= ~(3 << 11);
                /**1位/4位总线宽度 */
                SDIO->CLKCR |= WideMode;
                /**不开启硬件流控制 */
                SDIO->CLKCR |= 0 << 14;
            }
        }
    }
    return errorstatus;
}


//读取当前卡状态
//pcardstatus:卡状态
//返回值:错误代码
SD_Error DeviceSDIO_SendCmdStatus(uint32_t *pcardstatus)
{
    SD_Error errorstatus = SD_OK;
    if (pcardstatus == NULL)
    {
        errorstatus = SD_INVALID_PARAMETER;
        return errorstatus;
    }

    /**发送CMD13,短响应 */
    DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    /**查询响应状态 */
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SEND_STATUS);
    if (errorstatus != SD_OK)
        return errorstatus;

    *pcardstatus = SDIO->RESP1; //读取响应值
    return errorstatus;
}

//返回SD卡的状态
//返回值:SD卡状态
SDCardState DeviceSDIO_GetState(void)
{
	uint32_t resp1=0;
	if(DeviceSDIO_SendCmdStatus(&resp1)!=SD_OK)
        return SD_CARD_ERROR;
	else
        return (SDCardState)((resp1>>9) & 0x0F);
}



//初始化SD卡
//返回值:错误代码;(0,无错误)
SD_Error DeviceSDIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    SD_Error errorstatus = SD_OK;
    uint8_t clkdiv = 0;
    /**使能GPIOC,GPIOD*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
    /**SDIO时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

    /**SDIO复位 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);

    /**PC8,9,10,11,12复用功能输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    /**复用功能 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    /**100M */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    /**上拉 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    /**PC8,9,10,11,12复用功能输出 */
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /**PD2复用功能输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //引脚复用映射设置
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);

    /**SDIO结束复位 */
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);
    //SDIO外设寄存器设置为默认值
    DeviceSDIO_RegDeInit();

    /**不使用中断 */
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_SDIO_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_SDIO_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    /**SD卡上电 */
    errorstatus = DeviceSDIO_PowerON();
    if(errorstatus != SD_OK)
    {
        DeviceSDIO_PowerOFF();
        return errorstatus;
    }

    /**SD卡初始化 */
    errorstatus = DeviceSDIO_InitializeCards();
    if(errorstatus != SD_OK)
    {
        DeviceSDIO_PowerOFF();
        return errorstatus;
    }

    /**获取卡信息 */
    errorstatus = DeviceSDIO_GetCardInfo(&SDCardInfo);
    if(errorstatus != SD_OK)
    {
        DeviceSDIO_PowerOFF();
        return errorstatus;
    }

    /**选中SD卡 */
    errorstatus = DeviceSDIO_SelectDeselect((uint32_t)(SDCardInfo.RCA << 16));
    if(errorstatus != SD_OK)
    {
        DeviceSDIO_PowerOFF();
        return errorstatus;
    }

    /**设置位宽 4位宽度,如果是MMC卡,则不能用4位模式*/
    errorstatus = DeviceSDIO_EnableWideBus(DEVICE_SDIO_LINE_MODE);
    if ((errorstatus == SD_OK) || (SDIO_MULTIMEDIA_CARD == CardType))
    {
        if (SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1 || SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0)
        {
            /**V1.1/V2.0卡，设置最高48/4=12Mhz */
            clkdiv = SDIO_TRANSFER_CLK_DIV + 2;
        }
        else
        {
            /**SDHC等其他卡，设置最高48/2=24Mhz */
            clkdiv = SDIO_TRANSFER_CLK_DIV;
        }
        /**设置时钟频率,SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz */
        DeviceSDIO_ClockSet(clkdiv);
    }
    return errorstatus;
}

//得到NumberOfBytes以2为底的指数.
//NumberOfBytes:字节数.
//返回值:以2为底的指数值
static uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
	uint8_t count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
}

//SD卡读取一个块
//buf:读数据缓存区(必须4字节对齐!!)
//addr:读取地址
//blksize:块大小
SD_Error DeviceSDIO_ReadBlock(uint8_t *buf,long long addr,uint16_t blksize)
{
    SD_Error errorstatus = SD_OK;
    uint8_t power;
    /**转换为u32指针 */
    uint32_t count = 0, *tempbuff = (uint32_t *)buf;
    uint32_t timeout = SDIO_DATATIMEOUT;
    if (NULL == buf)
        return SD_INVALID_PARAMETER;
    /**数据控制寄存器清零(关DMA) */
    SDIO->DCTRL = 0x0;

    /**大容量卡 地址重新计算 */
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        blksize = 512;
        addr >>= 9;
    }
    /**清除DPSM状态机配置 */
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /**卡锁了 */
    if (SDIO->RESP1 & SD_CARD_LOCKED)
        return SD_LOCK_UNLOCK_FAILED;

    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);
        /**发送CMD16+设置数据长度为blksize,短响应 */
        DeviceSDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R1响应 */
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SET_BLOCKLEN);
        /**响应错误 */
        if (errorstatus != SD_OK)
            return errorstatus;
    }
    else
        return SD_INVALID_PARAMETER;

    /**清除DPSM状态机配置 */
    SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
    SDIO_DataInitStructure.SDIO_DataLength = blksize;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /**发送CMD17+从addr地址出读取数据,短响应 */
    DeviceSDIO_SendCommand(addr,SD_CMD_READ_SINGLE_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_READ_SINGLE_BLOCK);
    if (errorstatus != SD_OK)
        return errorstatus;

    /**关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
    INTX_DISABLE();
    /**无上溢/CRC/超时/完成(标志)/起始位错误 */
    while (!(SDIO->STA & ((1 << 5) | (1 << 1) | (1 << 3) | (1 << 10) | (1 << 9))))
    {
        /**接收区半满,表示至少存了8个字 */
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
        {
            /**循环读取数据 */
            for (count = 0; count < 8; count++)
            {
                *(tempbuff + count) = SDIO->FIFO;
            }
            tempbuff += 8;
            /**读数据溢出时间 */
            timeout = 0X7FFFFF;
        }
        /**处理超时 */
        else
        {
            if (timeout == 0)
                return SD_DATA_TIMEOUT;
            timeout--;
        }
    }
    errorstatus = DeviceSDIO_TransErrorProcess();
    if(errorstatus)
    {
        /**开启总中断 */
        INTX_ENABLE();
        /**清除所有标记 */
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        return errorstatus;
    }
    /**FIFO里面,还存在可用数据 */
    while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
    {
        /**循环读取数据 */
        *tempbuff = SDIO->FIFO;
        tempbuff++;
    }
    /**开启总中断 */
    INTX_ENABLE();
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return errorstatus;
}

//SD卡读取多个块
//buf:读数据缓存区
//addr:读取地址
//blksize:块大小
//nblks:要读取的块数
//返回值:错误状态

SD_Error DeviceSDIO_ReadMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks)
{
    SD_Error errorstatus = SD_OK;
    uint8_t power;
    uint32_t count = 0;
    uint32_t timeout = SDIO_DATATIMEOUT;
    /**转换为u32指针 */
    tempbuff = (uint32_t *)buf;

    /**数据控制寄存器清零(关DMA) */
    SDIO->DCTRL = 0x0;
    /**大容量卡 */
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        blksize = 512;
        addr >>= 9;
    }

    /**清除DPSM状态机配置 */
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /**卡锁了 */
    if (SDIO->RESP1 & SD_CARD_LOCKED)
        return SD_LOCK_UNLOCK_FAILED;

    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);

        /**发送CMD16+设置数据长度为blksize,短响应 */
        DeviceSDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R1响应 */
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SET_BLOCKLEN);
        if (errorstatus != SD_OK)
            return errorstatus; //响应错误
    }
    else
        return SD_INVALID_PARAMETER;

    /**多块读 */
    if (nblks > 1)
    {
        /**判断是否超过最大接收长度 */
        if (nblks * blksize > SD_MAX_DATA_LENGTH)
            return SD_INVALID_PARAMETER;

        /**nblks*blksize,512块大小,卡到控制器 */
        SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
        SDIO_DataInitStructure.SDIO_DataLength = nblks * blksize;
        SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
        SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
        SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
        SDIO_DataConfig(&SDIO_DataInitStructure);


        /**发送CMD18+从addr地址出读取数据,短响应 */
        DeviceSDIO_SendCommand(addr,SD_CMD_READ_MULT_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_READ_MULT_BLOCK);
        if (errorstatus != SD_OK)
            return errorstatus;

        /**关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
        INTX_DISABLE();
        /**无上溢/CRC/超时/完成(标志)/起始位错误 */
        while (!(SDIO->STA & ((1 << 5) | (1 << 1) | (1 << 3) | (1 << 8) | (1 << 9))))
        {
            /**接收区半满,表示至少存了8个字 */
            if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
            {
                /**循环读取数据 */
                for (count = 0; count < 8; count++)
                {
                    *(tempbuff + count) = SDIO->FIFO;
                }
                tempbuff += 8;
                /**读数据溢出时间 */
                timeout = 0X7FFFFF;
            }
            /**处理超时 */
            else
            {
                if (timeout == 0)
                    return SD_DATA_TIMEOUT;
                timeout--;
            }
        }
        errorstatus = DeviceSDIO_TransErrorProcess();
        if(errorstatus)
        {
            /**开启总中断 */
            INTX_ENABLE();
            /**清除所有标记 */
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);
            return errorstatus;
        }
        /**FIFO里面,还存在可用数据 */
        while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
        {
            *tempbuff = SDIO->FIFO;
            tempbuff++;
        }
        /**接收结束 */
        if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
        {
            if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
            {
                /**发送CMD12+结束传输 */
                DeviceSDIO_SendCommand(0,SD_CMD_STOP_TRANSMISSION,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_STOP_TRANSMISSION);
                if (errorstatus != SD_OK)
                    return errorstatus;
            }
        }
        /**开启总中断 */
        INTX_ENABLE();
        /**清除所有标记 */
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    }
    return errorstatus;
}

//读SD卡
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数
//返回值:错误状态;0,正常;其他,错误代码;
uint8_t DeviceSDIO_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t sta=SD_OK;
	long long lsector=sector;
	uint8_t n;
	lsector<<=9;
	if((uint32_t)buf%4!=0)
	{
        /**为对其考虑,操作FIFO必须四字节对其 */
	 	for(n=0;n<cnt;n++)
		{
            /**单个sector的读操作 */
		 	sta=DeviceSDIO_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);
            for(uint16_t index = 0; index < 512; index++)
            {
                buf[index] = SDIO_DATA_BUFFER[index];
            }
			buf+=512;
		}
	}else
	{
        /**单个sector的读操作 */
		if(cnt==1)
            sta=DeviceSDIO_ReadBlock(buf,lsector,512);
        /**多个sector*/
		else
            sta=DeviceSDIO_ReadMultiBlocks(buf,lsector,512,cnt);
	}
	return sta;
}

//检查卡是否正在执行写操作
//pstatus:当前状态.
//返回值:错误代码
SD_Error DeviceSDIO_IsCardProgramming(uint8_t *pstatus)
{
    volatile uint32_t respR1 = 0, status = 0;

    DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    status = SDIO->STA;

    /**等待操作完成 */
    while (!(status & ((1 << 0) | (1 << 6) | (1 << 2))))
        status = SDIO->STA;

    /**CRC检测失败 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_CMD_CRC_FAIL;
    }
    /**命令超时 */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    if (SDIO->RESPCMD != SD_CMD_SEND_STATUS)
        return SD_ILLEGAL_CMD;

    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    respR1 = SDIO->RESP1;
    *pstatus = (u8)((respR1 >> 9) & 0x0000000F);

    return SD_OK;
}

//SD卡写1个块
//buf:数据缓存区
//addr:写地址
//blksize:块大小
//返回值:错误状态
SD_Error DeviceSD_WriteBlock(uint8_t *buf,long long addr, uint16_t blksize)
{
    SD_Error errorstatus = SD_OK;
    uint8_t power = 0, cardstate = 0;
    uint32_t timeout = 0, bytestransferred = 0;
    uint32_t cardstatus = 0, count = 0, restwords = 0;
    uint32_t tlen = blksize; //总长度(字节)
    uint32_t *tempbuff = (uint32_t *)buf;

    /**参数错误 */
    if (buf == NULL)
        return SD_INVALID_PARAMETER;

    /**数据控制寄存器清零(关DMA) */
    SDIO->DCTRL = 0x0;

    /**清除DPSM状态机配置 */
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /**卡锁了 */
    if (SDIO->RESP1 & SD_CARD_LOCKED)
        return SD_LOCK_UNLOCK_FAILED;

    /**大容量卡 */
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        blksize = 512;
        addr >>= 9;
    }

    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);

        /**发送CMD16+设置数据长度为blksize,短响应 */
        DeviceSDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SET_BLOCKLEN);
        if (errorstatus != SD_OK)
            return errorstatus; //响应错误
    }
    else
        return SD_INVALID_PARAMETER;

    /**发送CMD13,查询卡的状态,短响应 */
    DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    /**等待R1响应 */
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SEND_STATUS);
    if (errorstatus != SD_OK)
        return errorstatus;

    cardstatus = SDIO->RESP1;
    timeout = SD_DATATIMEOUT;

    /**检查READY_FOR_DATA位是否置位 */
    while (((cardstatus & 0x00000100) == 0) && (timeout > 0))
    {
        timeout--;

        /**发送CMD13,查询卡的状态,短响应 */
        DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SEND_STATUS);
        if (errorstatus != SD_OK)
            return errorstatus;
        /**更新卡状态 */
        cardstatus = SDIO->RESP1;
    }
    /**超时 */
    if (timeout == 0)
        return SD_ERROR;

    /**发送CMD24,写单块指令,短响应 */
    DeviceSDIO_SendCommand(addr,SD_CMD_WRITE_SINGLE_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
    errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_WRITE_SINGLE_BLOCK);
    if (errorstatus != SD_OK)
        return errorstatus;

    /**blksize, 控制器到卡 */
    SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
    SDIO_DataInitStructure.SDIO_DataLength = blksize;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    timeout = SDIO_DATATIMEOUT;

    /**关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
    INTX_DISABLE();
    /**数据块发送成功/下溢/CRC/超时/起始位错误 */
    while (!(SDIO->STA & ((1 << 10) | (1 << 4) | (1 << 1) | (1 << 3) | (1 << 9))))
    {
        /**发送区半空,表示至少存了8个字 */
        if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
        {
            /**不够32字节了 */
            if ((tlen - bytestransferred) < SD_HALFFIFOBYTES)
            {
                restwords = ((tlen - bytestransferred) % 4 == 0) ? ((tlen - bytestransferred) / 4) : ((tlen - bytestransferred) / 4 + 1);
                for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
                {
                    SDIO->FIFO = *tempbuff;
                }
            }
            else
            {
                for (count = 0; count < 8; count++)
                {
                    SDIO->FIFO = *(tempbuff + count);
                }
                tempbuff += 8;
                bytestransferred += 32;
            }
            /**写数据溢出时间 */
            timeout = 0X3FFFFFFF;
        }
        else
        {
            if (timeout == 0)
                return SD_DATA_TIMEOUT;
            timeout--;
        }
    }
    errorstatus = DeviceSDIO_TransErrorProcess();
    if (errorstatus)
    {
        /**开启总中断 */
        INTX_ENABLE();
        /**清除所有标记 */
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        return errorstatus;
    }
    /**开启总中断 */
    INTX_ENABLE();
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    /**等待写入完成 */
    errorstatus = DeviceSDIO_IsCardProgramming(&cardstate);
    while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
    {
        errorstatus = DeviceSDIO_IsCardProgramming(&cardstate);
    }
    return errorstatus;
}

//SD卡写多个块
//buf:数据缓存区
//addr:写地址
//blksize:块大小
//nblks:要写入的块数
//返回值:错误状态
SD_Error DeviceSDIO_WriteMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks)
{
    SD_Error errorstatus = SD_OK;
    uint8_t power = 0, cardstate = 0;
    uint32_t timeout = 0, bytestransferred = 0;
    uint32_t count = 0, restwords = 0;
    /**总长度(字节) */
    uint32_t tlen = nblks * blksize;
    uint32_t *tempbuff = (uint32_t *)buf;

    /**参数错误 */
    if (buf == NULL)
        return SD_INVALID_PARAMETER;

    /**数据控制寄存器清零(关DMA) */
    SDIO->DCTRL = 0x0;

    /**清除DPSM状态机配置 */
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /**卡锁了 */
    if (SDIO->RESP1 & SD_CARD_LOCKED)
        return SD_LOCK_UNLOCK_FAILED;

    /**大容量卡 */
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        blksize = 512;
        addr >>= 9;
    }

    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);

        /**发送CMD16+设置数据长度为blksize,短响应 */
        DeviceSDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        /**等待R1响应 */
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SET_BLOCKLEN);
        /**响应错误 */
        if (errorstatus != SD_OK)
            return errorstatus;
    }
    else
        return SD_INVALID_PARAMETER;

    if (nblks > 1)
    {
        if (nblks * blksize > SD_MAX_DATA_LENGTH)
            return SD_INVALID_PARAMETER;
        if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
        {
            /**发送ACMD55,短响应 提高性能*/
            DeviceSDIO_SendCommand((uint32_t)RCA << 16,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_APP_CMD);
            if (errorstatus != SD_OK)
                return errorstatus;

            /**发送CMD23,设置块数量,短响应 */
            DeviceSDIO_SendCommand(nblks,SD_CMD_SET_BLOCK_COUNT,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_SET_BLOCK_COUNT);
            if (errorstatus != SD_OK)
                return errorstatus;
        }

        /**发送CMD25,多块写指令,短响应 */
        DeviceSDIO_SendCommand(addr,SD_CMD_WRITE_MULT_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_WRITE_MULT_BLOCK);
        if (errorstatus != SD_OK)
            return errorstatus;

        /**blksize, 控制器到卡 */
        SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
        SDIO_DataInitStructure.SDIO_DataLength = nblks * blksize;
        SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
        SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
        SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
        SDIO_DataConfig(&SDIO_DataInitStructure);

        timeout = SDIO_DATATIMEOUT;
        /**关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
        INTX_DISABLE();
        /**下溢/CRC/数据结束/超时/起始位错误 */
        while (!(SDIO->STA & ((1 << 4) | (1 << 1) | (1 << 8) | (1 << 3) | (1 << 9))))
        {
            /**发送区半空,表示至少存了8字(32字节) */
            if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
            {
                /**不够32字节了 */
                if ((tlen - bytestransferred) < SD_HALFFIFOBYTES)
                {
                    restwords = ((tlen - bytestransferred) % 4 == 0) ? ((tlen - bytestransferred) / 4) : ((tlen - bytestransferred) / 4 + 1);
                    for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
                    {
                        SDIO->FIFO = *tempbuff;
                    }
                }
                /**发送区半空,可以发送至少8字(32字节)数据 */
                else
                {
                    for (count = 0; count < SD_HALFFIFO; count++)
                    {
                        SDIO->FIFO = *(tempbuff + count);
                    }
                    tempbuff += SD_HALFFIFO;
                    bytestransferred += SD_HALFFIFOBYTES;
                }
                /**写数据溢出时间 */
                timeout = 0X3FFFFFFF;
            }
            else
            {
                if (timeout == 0)
                    return SD_DATA_TIMEOUT;
                timeout--;
            }
        }
        errorstatus = DeviceSDIO_TransErrorProcess();
        if (errorstatus)
        {
            /**开启总中断 */
            INTX_ENABLE();
            /**清除所有标记 */
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);
            return errorstatus;
        }
        /**发送结束 */
        if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
        {
            if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
            {
                /**发送CMD12+结束传输 */
                DeviceSDIO_SendCommand(0,SD_CMD_STOP_TRANSMISSION,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                errorstatus = DeviceSDIO_CheckCmdResp1(SD_CMD_STOP_TRANSMISSION);
                if (errorstatus != SD_OK)
                    return errorstatus;
            }
        }
        /**开启总中断 */
        INTX_ENABLE();
        /**清除所有标记 */
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    }
    /**清除所有标记 */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    /**等待写入完成 */
    errorstatus = DeviceSDIO_IsCardProgramming(&cardstate);
    while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
    {
        errorstatus = DeviceSDIO_IsCardProgramming(&cardstate);
    }
    return errorstatus;
}

//写SD卡
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数
//返回值:错误状态;0,正常;其他,错误代码;
uint8_t DeviceSDIO_WriteDisk(uint8_t* buf,uint32_t sector,uint8_t cnt)
{
    uint8_t sta=SD_OK;
	uint8_t n;
	long long lsector=sector;
	lsector<<=9;
	if((uint32_t)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
            for(uint16_t index = 0; index < 512; index++)
            {
                SDIO_DATA_BUFFER[index] = buf[index];
            }
            /**单个sector的写操作 */
		 	sta=DeviceSD_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);
			buf+=512;
		}
	}else
	{
        /**单个sector的写操作 */
		if(cnt==1)
            sta=DeviceSD_WriteBlock(buf,lsector,512);
        /**多个sector   */
		else
            sta=DeviceSDIO_WriteMultiBlocks(buf,lsector,512,cnt);
	}
	return sta;
}



//SDIO中断服务函数
void SDIO_IRQHandler(void)
{
    return;
}


















































