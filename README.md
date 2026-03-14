# -基于STM32与K210的酒精与疲劳驾驶检测-
use  stm32f103 as mcu use MQ-3 test for alcohol concentration and K210 test fatigue driving
## 项目功能
1. STM32采集MQ-3酒精传感器，实现酒驾检测
2. K210视觉识别，实现驾驶员疲劳检测
3. 串口通信联动，异常声光报警

## 硬件清单
- STM32F103C8T6
- K210 (MaixBit)
- MQ-3酒精传感器
- 蜂鸣器、LED

## 目录说明
- STM32_MCU：主控代码
- K210_AI：疲劳检测AI代码
- Hardware：接线图&硬件资料