/**
  ******************************************************************************
  * @file    Timer.c
  * @brief   定时器TIM2初始化配置文件
  * @author  STM32开发团队
  * @version V1.0
  * @date    2026-04-25
  * @note    本文件实现了TIM2定时器的初始化配置，用于产生1ms的定时中断
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header

/**
  * @brief  TIM2定时器初始化函数
  * @note   配置TIM2为向上计数模式，定时周期为1ms
  *         系统时钟频率为72MHz，通过预分频器和自动重装载值计算得到1ms定时
  *         计算公式: 定时周期 = 72000000 / (PSC+1) / (ARR+1)
  *         本配置: PSC = 71, ARR = 999 -> 定时周期 = 72000000/72/1000 = 1ms
  * @param  None
  * @retval None
  */
void Timer_Init(void)
{
    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 配置TIM2使用内部时钟作为时钟源
    TIM_InternalClockConfig(TIM2);
    
    // 定义定时器基本配置结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    
    // 设置时钟分频因子为1（不分频）
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    // 设置计数模式为向上计数模式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    // 设置自动重装载寄存器值(ARR)，决定定时周期
    // ARR = 1000 - 1 = 999，配合预分频器实现1ms定时
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
    
    // 设置预分频器值(PSC)
    // PSC = 72 - 1 = 71，将72MHz系统时钟分频为1MHz(72MHz/72=1MHz)
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    
    // 设置重复计数器值，高级定时器才需要，基本定时器设为0
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    // 初始化TIM2定时器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    // 清除TIM2的更新中断标志位，防止初始化后立即进入中断
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    // 使能TIM2的更新中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // 配置NVIC中断优先级分组为第2组（2位抢占优先级，2位响应优先级）
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 定义NVIC中断配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 设置中断通道为TIM2中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    
    // 使能中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    // 设置抢占优先级为2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    
    // 设置子优先级（响应优先级）为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    
    // 初始化NVIC配置
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能TIM2定时器，开始计数
    TIM_Cmd(TIM2, ENABLE);
}

/*
// TIM2中断服务函数（已注释掉）
// 如果需要使用定时器中断，请取消注释并在此函数中添加处理代码
void TIM2_IRQHandler(void) 
{
    // 检查是否是更新中断标志位被置位
    if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET) 
    {
        // 清除TIM2的更新中断挂起位
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update); 
        
        // 在此处添加定时器中断处理代码
        // 例如：LED闪烁、数据采集等周期性任务
    }
}
*/