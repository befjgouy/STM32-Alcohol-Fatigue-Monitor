#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void Timer_Init(void);           // TIM2 1ms 初始化
void SysTick_1ms_Tick(void);    // 1ms 系统时钟递增（中断里调用）

extern volatile uint32_t Sys_Millis;  // 系统毫秒计数（全局）

#endif