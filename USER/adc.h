#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"
#include "delay.h"

// 通用配置（可修改）
#define ADC_MAX_CHANNELS  8       // 最大支持8通道
#define ADC_RESOLUTION    4095.0f // 12位ADC
#define ADC_REF_VOLTAGE   3.3f    // 参考电压

// 通道定义（按硬件引脚）
typedef enum {
    ADC_CH0 = ADC_Channel_0,  // PA0
    ADC_CH1 = ADC_Channel_1,  // PA1
    ADC_CH2 = ADC_Channel_2,  // PA2
    ADC_CH3 = ADC_Channel_3,  // PA3
    ADC_CH4 = ADC_Channel_4,  // PA4
    ADC_CH5 = ADC_Channel_5,  // PA5
    ADC_CH6 = ADC_Channel_6,  // PA6
    ADC_CH7 = ADC_Channel_7   // PA7
} ADC_Channel_t;

// 函数声明
void ADC_myInit(void);
uint16_t ADC_ReadRaw(ADC_Channel_t ch);
float ADC_ReadVoltage(ADC_Channel_t ch);
uint16_t ADC_ReadAvg(ADC_Channel_t ch, uint8_t times);
float ADC_ReadVoltageAvg(ADC_Channel_t ch, uint8_t times);

#endif
