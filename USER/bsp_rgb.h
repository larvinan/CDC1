#ifndef __BSP_RGB_H
#define __BSP_RGB_H

#include "stm32f10x.h"

// RGB初始化
void RGB_Init(void);

// 设置RGB颜色 0~255
void RGB_SetColor(uint8_t r, uint8_t g, uint8_t b);

// 常用单色函数
void RGB_Red(void);    // 红
void RGB_Green(void);  // 绿
void RGB_Blue(void);   // 蓝
void RGB_White(void);  // 白
void RGB_Off(void);    // 熄灭

// 跳变彩虹（非渐变，直接切换颜色）
void RGB_Rainbow_Task(void);

// ===================== 新增功能 =====================
// 功能：检测RGB当前是否亮着
// 返回值：1=亮（任意颜色都算），0=全灭
uint8_t RGB_IsOn(void);

#endif

