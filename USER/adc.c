#include "adc.h"
#include "stm32f10x.h"
#include "delay.h"
/**
 * @brief  ADC初始化（单通道/多通道通用）
 */
void ADC_myInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    // 1. 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC时钟12MHz（72/6）

    // 2. 配置PA0~PA7为模拟输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. ADC1配置
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE; // 单通道模式
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStruct);

    // 4. 使能ADC并校准
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

/**
 * @brief  读取单次ADC原始值
 * @param  ch: 通道号（ADC_CH0~ADC_CH7）
 * @retval 12位ADC值（0~4095）
 */
uint16_t ADC_ReadRaw(ADC_Channel_t ch)
{
    // 配置通道
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_55Cycles5);
    // 软件启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // 等待转换完成
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    // 读取结果
    return ADC_GetConversionValue(ADC1);
}

/**
 * @brief  读取电压（V）
 */
float ADC_ReadVoltage(ADC_Channel_t ch)
{
    return (float)ADC_ReadRaw(ch) / ADC_RESOLUTION * ADC_REF_VOLTAGE;
}

/**
 * @brief  多次采样求平均（抗干扰）
 * @param  times: 采样次数（建议5~20）
 */
uint16_t ADC_ReadAvg(ADC_Channel_t ch, uint8_t times)
{
    uint32_t sum = 0;
		uint8_t i;
    for(i=0; i<times; i++)
    {
        sum += ADC_ReadRaw(ch);
       // delay_ms(1);
    }
    return sum / times;
}

/**
 * @brief  多次采样平均电压
 */
float ADC_ReadVoltageAvg(ADC_Channel_t ch, uint8_t times)
{
    return (float)ADC_ReadAvg(ch, times) / ADC_RESOLUTION * ADC_REF_VOLTAGE;
}


