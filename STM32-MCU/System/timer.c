#include "timer.h"
#include "stm32f10x.h"                  // Device header

volatile uint32_t Sys_Millis = 0;  // 系统毫秒计数器

// TIM2 1ms 中断初始化
void Timer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // TIM2 时钟
     // 计算说明：TIM2时钟=72MHz（APB1=36MHz×2），72MHz/7200=10kHz，10kHz/1000=1ms
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 10 - 1;      // 自动重装值（1ms）
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;   // 预分频（72MHz / 7200 = 10KHz）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 开启更新中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // NVIC 配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2, ENABLE);  // 启动定时器
}

// TIM2 中断服务函数（每 1ms 自动执行一次）
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        SysTick_1ms_Tick();  // 调用 1ms 系统时钟函数
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除中断标志
    }
}

// 1ms 系统时钟递增函数（供蜂鸣器等任务调用）
void SysTick_1ms_Tick(void)
{
    Sys_Millis++;  // 毫秒计数自增
}