// buzzer.h（头文件）
#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"
extern volatile uint32_t Sys_Millis;  // 你的毫秒计数器

// 蜂鸣器状态枚举
typedef enum {
    BUZZER_IDLE = 0,      // 空闲
    BUZZER_SINGLE_BEEP,   // 单次长鸣
    BUZZER_MULTI_BEEP     // 多次短鸣（滴滴滴）
} Buzzer_Mode_t;

// 蜂鸣器控制结构体
typedef struct {
    uint8_t Active;           // 是否激活（1=工作中，0=空闲）
    uint32_t StartTime;       // 开始时间（ms）
    uint32_t Duration;        // 当前阶段时长（ms）
    uint8_t BeepCount;        // 当前鸣叫次数
    uint8_t TargetBeeps;      // 目标鸣叫次数
    uint32_t SingleDuration;  // 单次长鸣时长
    Buzzer_Mode_t Mode;       // 工作模式
} Buzzer_t;

// 全局蜂鸣器对象
extern Buzzer_t Buzzer;

// 函数声明
void Buzz_Init(void);           // 你的原函数
void BUZZ_ON(void);             // 你的原函数
void BUZZ_OFF(void);            // 你的原函数

// 新增非堵塞控制函数
void Buzzer_Beep_Single(uint32_t duration_ms);       // 单次长鸣（酒精超标）
void Buzzer_Beep_Multiple(uint8_t times, uint32_t duration_each_ms); // 多次短鸣（疲劳）
void Buzzer_Task(void);  // 主循环必须调用（每10ms一次即可）

#endif