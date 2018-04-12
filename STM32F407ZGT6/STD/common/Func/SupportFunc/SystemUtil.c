#include "SystemUtil.h"

/*设置默认的中断优先级分组*/
void SystemUtilSetIntGroup(void)
{
    //设置系统默认中断分组
	NVIC_PriorityGroupConfig(SYSTEM_DEFAULT_NVIC_GROUP);
}

//THUMB指令不支持汇编内联  
//采用如下方法实现执行汇编指令WFI    
void WFI_SET(void)  
{  
    asm("WFI");         
}  
//关闭所有中断(但是不包括fault和NMI中断)  
void INTX_DISABLE(void)  
{         
    asm("CPSID   I");     
    asm("BX      LR");    
}  
//开启所有中断  
void INTX_ENABLE(void)  
{  
    asm("CPSIE   I");     
    asm("BX      LR");    
}  
//设置栈顶地址  
//addr:栈顶地址  
void MSR_MSP(u32 addr)   
{  
    asm("MSR MSP, r0"); //set Main Stack value  
    asm("BX r14");    
} 








