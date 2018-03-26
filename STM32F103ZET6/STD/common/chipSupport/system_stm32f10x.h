#ifndef __SYSTEM_STM32F10X_H
#define __SYSTEM_STM32F10X_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*!< System Clock Frequency (Core Clock) */
extern uint32_t SystemCoreClock;         


  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);


#ifdef __cplusplus
}
#endif

#endif 
