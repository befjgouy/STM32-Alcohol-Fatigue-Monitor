//#include "stm32f10x.h"                  // Device header

//void Buzz_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB,&GPIO_InitStruct);
//}

//void BUZZ_ON(void)
//{
//	GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);
//}

//void BUZZ_OFF(void)
//{
//	GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET);
//}
// buzzer.c（实现文件）
#include "buzzer.h"

Buzzer_t Buzzer = {0};  // 初始化蜂鸣器结构体
extern volatile uint32_t Sys_Millis;

// 你的原函数（保持不变）
void Buzz_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    BUZZ_OFF();  // 默认关闭（高电平）
}

void BUZZ_ON(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);  // 低电平 = 响
}

void BUZZ_OFF(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);    // 高电平 = 关
}

// 单次长鸣（酒精超标用，2秒）
void Buzzer_Beep_Single(uint32_t duration_ms)
{
    if (Buzzer.Active) return;  // 正在工作，忽略新请求
    
    BUZZ_ON();                          // 立即响
    Buzzer.StartTime = Sys_Millis;      // 记录开始时间
    Buzzer.Duration = duration_ms;      // 设置时长
    Buzzer.Active = 1;                  // 激活
    Buzzer.Mode = BUZZER_SINGLE_BEEP;   // 单次模式
    Buzzer.SingleDuration = duration_ms;
    Buzzer.BeepCount = 0;
    Buzzer.TargetBeeps = 0;
}

// 多次短鸣（疲劳用，滴滴滴3次，每次300ms）
void Buzzer_Beep_Multiple(uint8_t times, uint32_t duration_each_ms)
{
    if (Buzzer.Active) return;  // 正在工作，忽略
    
    BUZZ_ON();                          // 先响
    Buzzer.StartTime = Sys_Millis;
    Buzzer.Duration = duration_each_ms; // 每次鸣叫时长
    Buzzer.Active = 1;
    Buzzer.Mode = BUZZER_MULTI_BEEP;
    Buzzer.BeepCount = 0;
    Buzzer.TargetBeeps = times * 2;     // 响-停 各times次，共2倍切换
}

// 主任务函数（非堵塞，必须在主循环调用）
void Buzzer_Task(void)
{
    if (!Buzzer.Active) return;  // 空闲直接返回

    // 检查当前阶段时间是否到
    if ((Sys_Millis - Buzzer.StartTime) >= Buzzer.Duration)
    {
        if (Buzzer.Mode == BUZZER_SINGLE_BEEP)
        {
            // 单次鸣叫完成
            BUZZ_OFF();
            Buzzer.Active = 0;
        }
        else if (Buzzer.Mode == BUZZER_MULTI_BEEP)
        {
            Buzzer.BeepCount++;
            
            if (Buzzer.BeepCount >= Buzzer.TargetBeeps)
            {
                // 所有鸣叫完成
                BUZZ_OFF();
                Buzzer.Active = 0;
                Buzzer.BeepCount = 0;
            }
            else
            {
                // 切换状态：响→停 或 停→响
                if (Buzzer.BeepCount % 2 == 0)
                    BUZZ_ON();   // 偶数次：响
                else
                    BUZZ_OFF();  // 奇数次：停
                
                Buzzer.StartTime = Sys_Millis;  // 下一阶段重新计时
                Buzzer.Duration = (Buzzer.BeepCount < Buzzer.TargetBeeps - 1) ? 
                                  300 : 1000;  // 最后一次停1秒
            }
        }
    }
}