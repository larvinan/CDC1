#include "flame.h"

/**
 * @brief 火焰传感器DO口初始化（上拉输入，抗干扰）
 */
void Flame_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(FLAME_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = FLAME_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入，减少误触发
    GPIO_Init(FLAME_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief 读取火焰状态（仅判断有无，不读强度）
 * @retval 1: 检测到火焰/强红外光
 * @retval 0: 无火焰
 */
uint8_t Flame_IsDetected(void)
{
    // 火焰传感器特性：有火焰时DO输出低电平，无火焰时输出高电平
    return (GPIO_ReadInputDataBit(FLAME_GPIO_PORT, FLAME_GPIO_PIN) == 0) ? 1 : 0;
}


