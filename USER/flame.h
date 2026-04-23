#ifndef __FLAME_H
#define __FLAME_H
#include "stm32f10x.h"

// 引脚配置（推荐PB1，可直接修改这里换引脚）
#define FLAME_GPIO_PORT   GPIOB
#define FLAME_GPIO_PIN    GPIO_Pin_1
#define FLAME_RCC         RCC_APB2Periph_GPIOB

void Flame_Init(void);
uint8_t Flame_IsDetected(void); // 返回1=有火焰，0=无火焰

#endif
