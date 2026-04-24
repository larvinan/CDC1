/**
 * @file LDR.c
 * @brief LDR（光敏电阻）驱动
 * @details 本文件提供对连接到 ADC 通道的 LDR（光敏电阻）的初始化与读取接口，
 *          使用底层 ADC 接口（如 ADC_myInit、ADC_ReadAvg、ADC_ReadVoltageAvg）获取传感器数据。
 */

#include "ldr.h"
#include "adc.h"
#include "stm32f10x.h"

/**
 * @brief 初始化 LDR 相关外设（主要是 ADC）
 * @details 调用底层 ADC 初始化函数，配置对应的 GPIO/ADC 通道，以便后续读取 LDR 电压。
 */
void LDR_Init(void)
{
    // 初始化 ADC，使能采样通道和中断/时钟等（具体由 ADC_myInit 实现）
    ADC_myInit();
}

/**
 * @brief 读取 LDR 的原始 ADC 值（平均）
 * @return uint16_t 返回 ADC 原始值（根据底层实现可能为 10 位或 12 位幅值）
 * @details 使用 ADC_ReadAvg 对指定通道进行多次采样并返回平均值，以降低噪声。
 */
uint16_t LDR_Read(void)
{
    // ADC_CH6 为 LDR 所连接的 ADC 通道，采样 10 次并取平均
    return ADC_ReadAvg(ADC_CH6, 10);
}

/**
 * @brief 读取 LDR 对应的电压（平均）
 * @return float 返回电压值，单位为伏特（V）
 * @details 使用 ADC_ReadVoltageAvg 将 ADC 值换算为电压并进行平均处理。
 */
float LDR_ReadVoltage(void)
{
    return ADC_ReadVoltageAvg(ADC_CH6, 10);
}

/**
 * @brief 将 ADC 值转换为光照强度百分比（0-100%）
 * @return uint8_t 返回光照强度百分比，0 表示最暗，100 表示最亮
 * @details 简单线性映射：假定 ADC 最大值为 4095（12 位 ADC）。
 *          根据硬件接线方式，ADC 值与光照强度的关系可能为正相关或负相关，
 *          本函数遵循原有实现：light = 100 - (adc * 100 / 4095)。
 */
uint8_t LDR_ReadLight(void)
{
    uint16_t adc = LDR_Read();  // 读取 ADC 原始值（平均）
    // 将 ADC 值映射为 0-100%，并按原逻辑进行反向处理
    uint8_t light = 100 - (adc * 100 / 4095);
    return light;
}

