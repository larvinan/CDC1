#ifndef __SENSOR_LED_H
#define __SENSOR_LED_H
#include "stm32f10x.h"

// 初始化函数：调用一次即可
void Sensor_Led_Init(void);

// 获取人体感应状态 1=有人 0=无人
uint8_t Human_Read(void);


#endif
