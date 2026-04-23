#ifndef __MQ135_H
#define __MQ135_H

#include "stm32f10x.h"
#include "delay.h"

// -------------------------- 硬件引脚配置（根据你的电路修改） --------------------------
#define MQ135_AO_GPIO_PORT    GPIOA
#define MQ135_AO_GPIO_PIN     GPIO_Pin_0
#define MQ135_AO_GPIO_CLK     RCC_APB2Periph_GPIOA
#define MQ135_AO_ADC_CHANNEL  ADC_CH0
// ? 改成直接的数字（PA0对应通道0）
//#define MQ135_AO_ADC_CHANNEL  0

#define MQ135_DO_GPIO_PORT    GPIOA
#define MQ135_DO_GPIO_PIN     GPIO_Pin_1
#define MQ135_DO_GPIO_CLK     RCC_APB2Periph_GPIOA

// -------------------------- 传感器参数配置 --------------------------
#define MQ135_ADC_RESOLUTION  4095.0f    // 12位ADC满量程值
#define MQ135_REF_VOLTAGE     3.3f       // ADC参考电压3.3V
#define MQ135_RL              10.0f      // 模块负载电阻RL=10KΩ
//#define MQ135_RO_CLEAN_AIR    10.0f// 洁净空气中的基准电阻R0（需校准）
extern float MQ135_RO_CLEAN_AIR;
#define MQ135_SAMPLE_TIMES    10         // ADC采样次数（求平均）

// -------------------------- 函数声明 --------------------------
void MQ135_Init(void);                  // 传感器初始化
uint16_t MQ135_Read_ADC_Raw(void);      // 读取原始ADC值
float MQ135_Read_Voltage(void);         // 读取输出电压
float MQ135_Read_Rs(void);              // 计算传感器电阻Rs
uint8_t MQ135_Read_DO_State(void);      // 读取数字输出状态（报警）
float MQ135_Calibrate_R0(void);         // 校准洁净空气中的R0
float MQ135_Get_Air_Quality(void);      // 获取空气质量等级（0-100，数值越高质量越差）

#endif
