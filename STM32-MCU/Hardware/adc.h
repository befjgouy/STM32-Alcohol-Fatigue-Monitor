#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

void ADCx_Init(void);              // ADC 初始化
uint16_t ADC_GetValue(void);       // 获取 ADC 值（0-4095）
float ADC_GetVoltage(void);        // 获取电压值（0-3.3V）

#endif