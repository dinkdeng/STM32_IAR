#ifndef __CORE_SERIAL1_H_
#define __CORE_SERIAL1_H_
#include "stm32f10x.h"

/*232串口驱动程序*/

/*串口的缓冲区*/
extern uint8_t coreSerial1RXBuffer[64];

/** 保存当前串口接收数据状态,
 * 0-13位 数据长度
 * 14位 是否已经接收到回车
 * 15位 是否已经接收到换行. */
extern uint16_t coreSerial1RxStatus;

/*获取当前接收到的有效数据长度*/
#define CORE_UART1_GET_RECV_LENGTH()   ((coreSerial1RxStatus&0x8000) ?\
                                            coreSerial1RxStatus&0x00FF : 0)

/*清除当前串口接受状态*/
#define CORE_UART1_CLEAR_STATUS()         (coreSerial1RxStatus = 0)

/*检查当前串口有没有接收到回车*/
#define CORE_UART1_CHECK_RX_STATUS_CR()   (coreSerial1RxStatus&0x4000)

/*设置当前串口接收到回车*/
#define CORE_UART1_SET_RX_STATUS_CR()     (coreSerial1RxStatus |= 0x4000)

/*检查当前串口有没有接收到换行*/
#define CORE_UART1_CHECK_RX_STATUS_LF()   (coreSerial1RxStatus&0x8000)

/*设置当前串口接收到换行*/
#define CORE_UART1_SET_RX_STATUS_LF()     (coreSerial1RxStatus |= 0x8000)


void CoreSerial1Init(uint32_t bpsSet);

void CoreSerial1SendByte(uint8_t dat);

void CoreSerial1SendBuffer(uint8_t* datPtr,uint16_t length);

void CoreSerial1SendString(uint8_t* strPtr);



#endif





