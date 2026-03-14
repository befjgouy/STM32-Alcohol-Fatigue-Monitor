#include "adc.h"
#include "stm32f10x.h"                  // Device header

void ADCx_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // GPIO 配置：PA0 模拟输入
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ADC 配置
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;              // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                   // 单通道
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;             // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;          // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                         // 1个通道
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // 配置规则通道（通道0，采样时间）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    
    // 开启 ADC
    ADC_Cmd(ADC1, ENABLE);
    
    // 校准 ADC（必须）
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC_GetValue(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);           // 软件触发转换
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));   // 等待转换结束
    return ADC_GetConversionValue(ADC1);              // 返回 12 位值 (0-4095)
}

float ADC_GetVoltage(void)
{
    uint16_t adc_value = ADC_GetValue();
    return (adc_value * 3.3f) / 4095.0f;              // 转换为电压
}