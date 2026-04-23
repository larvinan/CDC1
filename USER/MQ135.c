#include "mq135.h"
#include "adc.h"  // 依赖你已有的ADC驱动，若没有可直接用下面的ADC读取函数
#include "stm32f10x.h"
#include "delay.h"
// -------------------------- 内部辅助函数 --------------------------
/* 读取ADC单次值（若已有ADC驱动可替换）
static uint16_t MQ135_ADC_GetValue(uint8_t ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}
*/
// -------------------------- 对外接口实现 --------------------------
/**
 * @brief  MQ135传感器初始化
 * @param  无
 * @retval 无
 */
 float MQ135_RO_CLEAN_AIR = 20.0f;
void MQ135_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 1. 使能GPIO和ADC时钟
    RCC_APB2PeriphClockCmd(MQ135_AO_GPIO_CLK | MQ135_DO_GPIO_CLK | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // ADC时钟配置为72MHz/6=12MHz

    // 2. 配置AO引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = MQ135_AO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(MQ135_AO_GPIO_PORT, &GPIO_InitStructure);

    // 3. 配置DO引脚为上拉输入
    GPIO_InitStructure.GPIO_Pin = MQ135_DO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MQ135_DO_GPIO_PORT, &GPIO_InitStructure);

    // 4. 配置ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 5. 使能ADC
    ADC_Cmd(ADC1, ENABLE);

    // 6. 传感器预热提示（实际使用需等待24小时完全稳定，这里仅做初始化）
    delay_ms(100);
}

/**
 * @brief  读取ADC原始值（多次采样求平均）
 * @param  无
 * @retval ADC平均值
 */
uint16_t MQ135_Read_ADC_Raw(void)
{
    uint32_t adc_sum = 0;
    uint8_t i;

    for(i = 0; i < MQ135_SAMPLE_TIMES; i++)
    {
        adc_sum += ADC_ReadAvg(MQ135_AO_ADC_CHANNEL,10);
        delay_ms(5);
    }

    return (uint16_t)(adc_sum / MQ135_SAMPLE_TIMES);
}

/**
 * @brief  读取传感器输出电压
 * @param  无
 * @retval 电压值(V)
 */
float MQ135_Read_Voltage(void)
{
    uint16_t adc_val = MQ135_Read_ADC_Raw();
    return (float)adc_val / MQ135_ADC_RESOLUTION * MQ135_REF_VOLTAGE;
}

/**
 * @brief  计算传感器电阻Rs
 * @param  无
 * @retval Rs值(KΩ)
 */
float MQ135_Read_Rs(void)
{
    float voltage = MQ135_Read_Voltage();
    // Rs = RL * (Vc - Vout) / Vout
    return MQ135_RL * (MQ135_REF_VOLTAGE - voltage) / voltage;
}

/**
 * @brief  读取数字输出状态（DO）
 * @param  无
 * @retval 0: 气体浓度超过阈值（报警），1: 正常
 */
uint8_t MQ135_Read_DO_State(void)
{
    return GPIO_ReadInputDataBit(MQ135_DO_GPIO_PORT, MQ135_DO_GPIO_PIN);
}

/**
 * @brief  校准洁净空气中的基准电阻R0（必须在洁净空气环境中执行）
 * @param  无
 * @retval 校准后的R0值(KΩ)
 */
float MQ135_Calibrate_R0(void)
{
    float rs_sum = 0;
    uint8_t i;

    // 传感器预热20分钟后，在洁净空气中采样10次求平均
    for(i = 0; i < 10; i++)
    {
        rs_sum += MQ135_Read_Rs();
        delay_ms(500);
    }

    MQ135_RO_CLEAN_AIR = rs_sum / 10.0f;
    return MQ135_RO_CLEAN_AIR;
}

/**
 * @brief  获取空气质量等级（简化版，数值0-100，越大越差）
 * @param  无
 * @retval 空气质量分数
 */
float MQ135_Get_Air_Quality(void)
{
    float rs = MQ135_Read_Rs();
    // Rs/R0比值越大，空气质量越差，映射到0-100
    float ratio = rs/ MQ135_RO_CLEAN_AIR ;
    if(ratio < 1.0f) ratio = 1.0f;
    if(ratio > 10.0f) ratio = 10.0f;

    return (ratio - 1.0f) * 11.1f;  // 1→0，10→100
}
