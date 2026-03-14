//#include "stm32f10x.h"                  // Device header
//#include "Delay.h"
//#include "OLED.h"
//#include "Serial.h"
//#include "LED.h"
//#include "string.h"
//#include "adc.h"
//#include "buzzer.h"
//#include "timer.h"

//extern uint8_t K210_Status;  // 声明：这个变量在其他文件里定义
//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	LED_Init();			//LED初始化
//  Serial_Init();		//串口初始化
//	Buzz_Init();//蜂鸣器初始化
//	Timer_Init();
//	OLED_ShowCHinese(0,0,0);OLED_ShowCHinese(0,16,1);
//	OLED_ShowCHinese(0,32,2);OLED_ShowCHinese(0,48,3);//系统启动
//	USART2_Init();
//	ADCx_Init();        // 新增：初始化 ADC (MQ-3)

//	
//	// 测试用例，0.96显示屏(128 X 64) 最多显示4行汉子(0行开始)，每个汉字位宽/高均为16像素
//	//OLED_ShowCHinese(0, 0, 0);	
//	//OLED_ShowCHinese(2, 32, 0);	
//  //OLED_ShowCHinese(4, 0, 0);					  	//杭
//	//OLED_ShowCHinese(6, 16, 1); 						//州
//	
//	float alcohol_voltage = 0.0f;
//  uint8_t alcohol_alert = 0;  // 0:正常 1:超标                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
//  
//	while (1)
//	{
//	
//		// 读取 MQ-3 电压（每秒测一次）
//				Buzzer_Task();
//		
//		
//        alcohol_voltage = ADC_GetVoltage();
//        
//        // 阈值判断（可根据实际测试调整）
//        // 正常空气 ≈ 0.3-0.5V，喝酒后吹气 ≈ 1.0-2.5V
//        if(alcohol_voltage > 1.2f)  // 建议阈值 1.2V（可现场校准）
//        {
//            alcohol_alert = 1;
//        }
//        else
//        {
//            alcohol_alert = 0;
//        }

//        // 调试打印（通过 USART1）
//        Serial_Printf("Alcohol Voltage: %.2fV  ", alcohol_voltage);
//        if(alcohol_alert)
//            Serial_Printf("Alcohol level is over\r\n");
//        else
//            Serial_Printf("Alcohol level is normal\r\n");

//        Serial_Printf("K210 Status: %c\r\n", K210_Status);

//        // 综合报警判断（后续加蜂鸣器、OLED 用这个）
//        if(alcohol_alert || K210_Status == 'F')
//        {
//					Serial_Printf("Warning:Please don't drive!\r\n");
//            // 这里后续接蜂鸣器、LED
//        }

//        Delay_ms(1000);  // 每秒检测一次
//	}
//}

//#include "stm32f10x.h"
//#include "Delay.h"
//#include "OLED.h"
//#include "Serial.h"
//#include "LED.h"
//#include "string.h"
//#include "adc.h"
//#include "buzzer.h"
//#include "timer.h"

//extern uint8_t K210_Status;  // K210状态：'F'代表疲劳，其他代表正常
//extern volatile uint32_t Sys_Millis; // 引用定时器里的毫秒计数器

//int main(void)
//{
//    /* 模块初始化 */
//    OLED_Init();        
//    LED_Init();         
//    Serial_Init();      
//    Buzz_Init();        // 蜂鸣器初始化
//    Timer_Init();       // 1ms 定时器初始化
//    USART2_Init();
//    ADCx_Init();        

//    // 系统启动提示
////    OLED_ShowCHinese(0,0,0); OLED_ShowCHinese(0,16,1);
////    OLED_ShowCHinese(0,32,2); OLED_ShowCHinese(0,48,3); 
//	OLED_ShowString(1,1,"systemstart");
//	OLED_ShowString(2,1,"alchoal:");
//    // 启动时响三声，表示系统正常启动
//    Buzzer_Beep_Multiple(3, 200);

//    float alcohol_voltage = 0.0f;
//    uint8_t alcohol_alert = 0;
//    
//    uint32_t Last_Check_Time = 0; // 用于替代 Delay_ms(1000)

//    while (1)
//    {
//        // 【核心】必须在循环最开始调用，保证蜂鸣器状态实时更新
//        Buzzer_Task();

//        // 使用时间戳判断，每 1000ms 执行一次检测逻辑，不阻塞 CPU
//        if (Sys_Millis - Last_Check_Time >= 1000)
//        {
//            Last_Check_Time = Sys_Millis; // 更新最后检查时间

//            // 1. 读取 MQ-3 电压
//            alcohol_voltage = ADC_GetVoltage();
//            
//            // 2. 阈值判断
//            if(alcohol_voltage > 1.2f) 
//                alcohol_alert = 1;
//            else
//                alcohol_alert = 0;

//            // 3. 串口调试信息
//            Serial_Printf("Alcohol: %.2fV | K210: %c\r\n", alcohol_voltage, K210_Status);

//            // 4. 报警逻辑触发
//            if(alcohol_alert)
//            {
//                // 酒精超标：触发单次长鸣 (例如 2 秒)
//                // 注意：Buzzer_Beep 函数内部有 Active 判断，如果正在响则不会重复触发
//                Buzzer_Beep_Single(2000); 
//                Serial_Printf("Warning: Alcohol Over! Don't drive!\r\n");
//            }
//            else if(K210_Status == 'F')
//            {
//                // 疲劳驾驶：触发多次短鸣 (滴-滴-滴，3次，每次300ms)
//                Buzzer_Beep_Multiple(3, 300);
//                Serial_Printf("Warning: Fatigue Detected!\r\n");
//            }
//        }

//        // 这里可以继续处理其他实时性要求高的任务
//        // 比如 OLED 刷新、按键扫描等
//    }
//}
#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "string.h"
#include "adc.h"
#include "buzzer.h"
#include "timer.h"
#include <stdio.h> // 用于 sprintf 格式化字符串

extern uint8_t K210_Status;  
extern volatile uint32_t Sys_Millis; 

int main(void)
{
    char display_buf[17]; // 用于存放格式化后的字符串（16字符+1结束符）

    /* 模块初始化 */
    OLED_Init();        
    LED_Init();         
    Serial_Init();      
    Buzz_Init();        
    Timer_Init();       
    USART2_Init();
    ADCx_Init();        

    // 1. 静态界面初始化（只显示一次，节省资源）
    OLED_Clear();
    OLED_ShowString(1, 1, "Safe Driving Sys"); // 第一行：标题
    OLED_ShowString(2, 1, "Alc: 0.00V");       // 第二行：酒精电压预设
    OLED_ShowString(3, 1, "K210: OK");         // 第三行：K210状态预设
    OLED_ShowString(4, 1, "Status: Normal");   // 第四行：系统状态
    
    // 启动提示音
    Buzzer_Beep_Multiple(2, 150);

    float alcohol_voltage = 0.0f;
    uint32_t Last_Check_Time = 0;

    while (1)
    {
        // 【核心】必须实时调用，驱动蜂鸣器状态机
        Buzzer_Task();

        // 每 500ms 刷新一次数据（比1秒快一点，显示更丝滑）
        if (Sys_Millis - Last_Check_Time >= 500)
        {
            Last_Check_Time = Sys_Millis;

            // --- 1. 数据采集 ---
            alcohol_voltage = ADC_GetVoltage();

            // --- 2. 逻辑判断与报警触发 ---
            if(alcohol_voltage > 1.2f) 
            {
                Buzzer_Beep_Single(2000); // 酒精超标长鸣
                OLED_ShowString(4, 1, "Status: ALC WARN"); // 第4行显示报警
            }
            else if(K210_Status == 'F')
            {
                Buzzer_Beep_Multiple(3, 300); // 疲劳驾驶短鸣
                OLED_ShowString(4, 1, "Status: FATIGUE "); 
            }
            else
            {
                OLED_ShowString(4, 1, "Status: Normal  "); // 正常
            }

            // --- 3. OLED 数据动态更新 ---
            
            // 更新第二行：酒精电压
            // 使用 sprintf 将浮点数转为字符串
            sprintf(display_buf, "Alc: %.2fV    ", alcohol_voltage); 
            OLED_ShowString(2, 1, display_buf);

            // 更新第三行：K210 状态
            if(K210_Status == 'F') {
                OLED_ShowString(3, 1, "K210: FATIGUE!");
            } else {
                sprintf(display_buf, "K210: OK (%c)   ", K210_Status);
                OLED_ShowString(3, 1, display_buf);
            }

            // --- 4. 串口同步输出 ---
            Serial_Printf("V:%.2fV, K210:%c\r\n", alcohol_voltage, K210_Status);
        }
    }
}

