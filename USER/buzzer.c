#include "BUZZER.h" 

/************************* 硬件配置区（移植仅修改这里） *************************/
// 1. GPIO 端口 / 引脚 / 时钟配置
#define BEEP_GPIO_PORT       GPIOB               // 蜂鸣器GPIO端口
#define BEEP_GPIO_PIN        GPIO_Pin_5          // 蜂鸣器GPIO引脚
#define BEEP_GPIO_RCC        RCC_APB2Periph_GPIOB // 对应时钟

// 2. 触发电平配置（根据硬件电路选择）
// 1=高电平鸣叫  0=低电平鸣叫
#define BEEP_TRIGGER_LEVEL   0
/******************************************************************************/

/**
 * @brief  蜂鸣器GPIO初始化（内部调用）
 * @retval 无
 */
static void Beep_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 开启GPIO时钟
    RCC_APB2PeriphClockCmd(BEEP_GPIO_RCC, ENABLE);

    // GPIO配置：推挽输出、50MHz速率
    GPIO_InitStruct.GPIO_Pin   = BEEP_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief  蜂鸣器总初始化函数
 * @retval 无
 */
void Beep_Init(void)
{
    Beep_GPIO_Init();  // 初始化IO口
    Beep_Stop();       // 开机默认关闭蜂鸣器
}

/**
 * @brief  开启蜂鸣器（持续响）
 * @retval 无
 */
void Beep_Start(void)
{
    // 根据配置的触发电平控制引脚
    if(BEEP_TRIGGER_LEVEL == 1)
    {
        GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    }
    else
    {
        GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    }
}

/**
 * @brief  关闭蜂鸣器（停止响）
 * @retval 无
 */
void Beep_Stop(void)
{
    // 输出与触发电平相反的电平，关闭蜂鸣器
    if(BEEP_TRIGGER_LEVEL == 1)
    {
        GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    }
    else
    {
        GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    }
}


