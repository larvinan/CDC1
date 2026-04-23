#include "ldr.h"
#include "adc.h"      // 只调用，不修改
#include "stm32f10x.h"
// 初始化 LDR（PA3）
void LDR_Init(void)
{
    // 直接使用你现有的ADC初始化，不改动底层
    ADC_myInit();
}

// 读取 PA3 通道 ADC 原始值（10次平均）
uint16_t LDR_Read(void)
{
    return ADC_ReadAvg(ADC_CH6, 10);
}

// 读取 PA3 电压值
float LDR_ReadVoltage(void)
{
    return ADC_ReadVoltageAvg(ADC_CH6, 10);
}

// 读取光照强度百分比 0~100%
// 100% = 最亮
// 0% = 最暗
uint8_t LDR_ReadLight(void)
{
    uint16_t adc = LDR_Read();
    uint8_t light = 100 - (adc * 100 / 4095);
    return light;
}
