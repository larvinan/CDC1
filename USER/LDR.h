#ifndef __LDR_H
#define __LDR_H

#include "stm32f10x.h"

// LDR 光敏电阻 PA3 专用函数
void LDR_Init(void);            // 初始�?
uint16_t LDR_Read(void);        // 读取原始ADC�?0~4095
float LDR_ReadVoltage(void);    // 读取电压
uint8_t LDR_ReadLight(void);    // 读取光照强度 0~100%

#endif


