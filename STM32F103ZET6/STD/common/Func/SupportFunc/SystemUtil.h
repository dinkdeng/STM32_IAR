#ifndef __SYSTEM_UTIL_H_
#define __SYSTEM_UTIL_H_
#include "stm32f10x.h"

/*主板程序运行需要的一些支持,例如位段操作,中断优先级,以及其他一些辅助函数*/

/*设置默认的中断优先级分组*/
void SystemUtilSetIntGroup(void);


/*主板中断分组设计*/
/*可选值 : NVIC_PriorityGroup_0 NVIC_PriorityGroup_1 NVIC_PriorityGroup_2
 NVIC_PriorityGroup_3 NVIC_PriorityGroup_4*/
#define BOARD_DEFAULT_NVIC_GROUP    NVIC_PriorityGroup_2

/*空引用定义*/
#ifndef NULL
#define NULL    0
#endif

/*中断枚举定义*/
/*系统中断优先级定义*/
/*NVIC中断分组以及系统中中断优先级的定义*/
/*数值越小,优先级越高*/
typedef enum INT_PRE_PRI
{
    INT_PRE_PRI_0 = 0,
    INT_PRE_PRI_1 = 1,
    INT_PRE_PRI_2 = 2,
    INT_PRE_PRI_3 = 3,
}INT_PRE_PRI;

//子优先级
typedef enum INT_SUB_PRI
{
    INT_SUB_PRI_0 = 0,
    INT_SUB_PRI_1 = 1,
    INT_SUB_PRI_2 = 2,
    INT_SUB_PRI_3 = 3,
}INT_SUB_PRI;


/*系统位段支持*/
/*位段模式操作IO口定义*/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

/*端口输出寄存器地址定义*/
/*0x4001080C*/
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) 
/*0x40010C0C*/
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) 
/*0x4001100C*/
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) 
/*0x4001140C*/
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) 
/*0x4001180C*/
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) 
/*0x40011A0C*/
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) 
/*0x40011E0C*/
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) 

/*端口输入寄存器地址定义*/
/*0x40010808*/
#define GPIOA_IDR_Addr    (GPIOA_BASE+8) 
/*0x40010C08*/
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) 
/*0x40011008*/
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) 
/*0x40011408*/
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) 
/*0x40011808*/
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) 
/*0x40011A08*/
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) 
/*0x40011E08*/
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) 

/*端口位段映射*/
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)

#endif





