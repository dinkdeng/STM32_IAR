/**1.2.0固件库版本 */
#ifndef __SYSTEM_STM32F7XX_H
#define __SYSTEM_STM32F7XX_H

#ifdef __cplusplus
 extern "C" {
#endif 

/**导出系统内核时钟变量 */
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

/**导出分频器值对应的分频系数 */
extern const uint8_t  AHBPrescTable[16];    /*!< AHB prescalers table values */
extern const uint8_t  APBPrescTable[8];     /*!< APB prescalers table values */


  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);


#ifdef __cplusplus
}
#endif

#endif
