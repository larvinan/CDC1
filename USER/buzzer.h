#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"  // STM32标准库核心头文件
#include "stdint.h"     // 标准数据类型

/************************* 函数声明 *************************/
// 蜂鸣器初始化（GPIO初始化+默认关闭）
void Beep_Init(void);

// 蜂鸣器开启（持续鸣叫）
void Beep_Start(void);

// 蜂鸣器关闭（停止鸣叫）
void Beep_Stop(void);

// 蜂鸣器鸣叫指定时长（毫秒）
void Beep_AlarmDuration(uint32_t delay_ms);

// 蜂鸣器间歇报警（滴滴滴）
void Beep_AlarmIntermittent(uint8_t cycle_num, uint16_t on_ms, uint16_t off_ms);

#endif



