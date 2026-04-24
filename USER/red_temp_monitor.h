#ifndef __IR_OBSTACLE_H
#define __IR_OBSTACLE_H

#include "stm32f10x.h"

// ------------ 引脚定义（根据你的硬件修改）------------
// 红外发射模块 HW-489
#define IR_TX_PORT      GPIOC
#define IR_TX_PIN       GPIO_Pin_14
#define IR_TX_CLK       RCC_APB2Periph_GPIOC

// 红外接收模块 HW-490
#define IR_RX_PORT      GPIOC
#define IR_RX_PIN       GPIO_Pin_15
#define IR_RX_CLK       RCC_APB2Periph_GPIOC

// 消抖时间（ms），防止误触发
#define IR_DEBOUNCE_MS  20

/**
 * @brief  红外发射+接收模块初始化
 */
void IR_Obstacle_Init(void);

/**
 * @brief  红外遮挡检测函数
 * @retval 1=有人挡住光路（检测到人员），0=无人遮挡
 */
uint8_t IR_Detect_Person(void);

#endif


