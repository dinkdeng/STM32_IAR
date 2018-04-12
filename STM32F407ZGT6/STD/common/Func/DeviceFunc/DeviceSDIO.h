#ifndef __DEVICE_SDIO_H_
#define __DEVICE_SDIO_H_
#include "stm32f4xx.h"
#include "SystemUtil.h"
#include "CoreTickDelay.h"

//支持的SD卡定义 类型
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)

//用户配置区
//SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz
//使用DMA模式的时候,传输速率可以到48Mhz(bypass on时),不过如果你的卡不是高速
//卡,可能也会出错,出错就请降低时钟
#define SDIO_INIT_CLK_DIV        0x86 		//SDIO初始化频率，最大400Kh

/**SDIO传输频率,该值太小可能会导致读写文件出错,根据实际电路调整*/
#define SDIO_TRANSFER_CLK_DIV    0x00

/**定义SD卡使用单线模式或者四线模式 */
#define DEVICE_SDIO_LINE_MODE		SDIO_BusWide_1b

/**SDIO中断优先级定义 */
#define DEVICE_SDIO_PREE_PRI        INT_PRE_PRI_1
#define DEVICE_SDIO_SUB_PRI         INT_SUB_PRI_0


//SDIO 各种错误枚举定义
typedef enum
{
	//特殊错误定义
	SD_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
	SD_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
	SD_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
	SD_DATA_TIMEOUT                    = (4), /*!< Data time out */
	SD_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
	SD_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
	SD_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
	SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
	SD_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
	SD_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
	SD_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
	SD_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
	SD_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
	SD_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
	SD_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
	SD_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
	SD_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
	SD_CC_ERROR                        = (18), /*!< Internal card controller error */
	SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
	SD_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
	SD_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
	SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD overwrite error */
	SD_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
	SD_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
	SD_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
	SD_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
	SD_INVALID_VOLTRANGE               = (27),
	SD_ADDR_OUT_OF_RANGE               = (28),
	SD_SWITCH_ERROR                    = (29),
	SD_SDIO_DISABLED                   = (30),
	SD_SDIO_FUNCTION_BUSY              = (31),
	SD_SDIO_FUNCTION_FAILED            = (32),
	SD_SDIO_UNKNOWN_FUNCTION           = (33),
	//标准错误定义
	SD_INTERNAL_ERROR,
	SD_NOT_CONFIGURED,
	SD_REQUEST_PENDING,
	SD_REQUEST_NOT_APPLICABLE,
	SD_INVALID_PARAMETER,
	SD_UNSUPPORTED_FEATURE,
	SD_UNSUPPORTED_HW,
	SD_ERROR,
	SD_OK = 0
} SD_Error;


//SD卡CSD寄存器数据
typedef struct
{
	u8  CSDStruct;            /*!< CSD structure */
	u8  SysSpecVersion;       /*!< System specification version */
	u8  Reserved1;            /*!< Reserved */
	u8  TAAC;                 /*!< Data read access-time 1 */
	u8  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
	u8  MaxBusClkFrec;        /*!< Max. bus clock frequency */
	u16 CardComdClasses;      /*!< Card command classes */
	u8  RdBlockLen;           /*!< Max. read data block length */
	u8  PartBlockRead;        /*!< Partial blocks for read allowed */
	u8  WrBlockMisalign;      /*!< Write block misalignment */
	u8  RdBlockMisalign;      /*!< Read block misalignment */
	u8  DSRImpl;              /*!< DSR implemented */
	u8  Reserved2;            /*!< Reserved */
	u32 DeviceSize;           /*!< Device Size */
	u8  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
	u8  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
	u8  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
	u8  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
	u8  DeviceSizeMul;        /*!< Device size multiplier */
	u8  EraseGrSize;          /*!< Erase group size */
	u8  EraseGrMul;           /*!< Erase group size multiplier */
	u8  WrProtectGrSize;      /*!< Write protect group size */
	u8  WrProtectGrEnable;    /*!< Write protect group enable */
	u8  ManDeflECC;           /*!< Manufacturer default ECC */
	u8  WrSpeedFact;          /*!< Write speed factor */
	u8  MaxWrBlockLen;        /*!< Max. write data block length */
	u8  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
	u8  Reserved3;            /*!< Reserded */
	u8  ContentProtectAppli;  /*!< Content protection application */
	u8  FileFormatGrouop;     /*!< File format group */
	u8  CopyFlag;             /*!< Copy flag (OTP) */
	u8  PermWrProtect;        /*!< Permanent write protection */
	u8  TempWrProtect;        /*!< Temporary write protection */
	u8  FileFormat;           /*!< File Format */
	u8  ECC;                  /*!< ECC code */
	u8  CSD_CRC;              /*!< CSD CRC */
	u8  Reserved4;            /*!< always 1*/
} SD_CSD;

//SD卡CID寄存器数据
typedef struct
{
	u8  ManufacturerID;       /*!< ManufacturerID */
	u16 OEM_AppliID;          /*!< OEM/Application ID */
	u32 ProdName1;            /*!< Product Name part1 */
	u8  ProdName2;            /*!< Product Name part2*/
	u8  ProdRev;              /*!< Product Revision */
	u32 ProdSN;               /*!< Product Serial Number */
	u8  Reserved1;            /*!< Reserved1 */
	u16 ManufactDate;         /*!< Manufacturing Date */
	u8  CID_CRC;              /*!< CID CRC */
	u8  Reserved2;            /*!< always 1 */
} SD_CID;

//SD卡状态
typedef enum
{
	SD_CARD_READY                  = ((uint32_t)0x00000001),
	SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
	SD_CARD_STANDBY                = ((uint32_t)0x00000003),
	SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
	SD_CARD_SENDING                = ((uint32_t)0x00000005),
	SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
	SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
	SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
	SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;

//SD卡信息,包括CSD,CID等数据
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  long long CardCapacity;  	//SD卡容量,单位:字节,最大支持2^64字节大小的卡.
  u32 CardBlockSize; 		//SD卡块大小
  u16 RCA;					//卡相对地址
  u8 CardType;				//卡类型
} SD_CardInfo;

extern SD_CardInfo SDCardInfo;//SD卡信息

//初始化SD卡
//返回值:错误代码;(0,无错误)
SD_Error DeviceSDIO_Init(void);

//读SD卡
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数
//返回值:错误状态;0,正常;其他,错误代码;
uint8_t DeviceSDIO_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);

//写SD卡
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数
//返回值:错误状态;0,正常;其他,错误代码;
uint8_t DeviceSDIO_WriteDisk(uint8_t* buf,uint32_t sector,uint8_t cnt);







#endif






