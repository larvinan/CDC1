#include "stm32f10x.h"                  // Device header

void Timer_Init ()
{
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2,ENABLE);//开启时钟
	TIM_InternalClockConfig (TIM2);//使用内部时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//滤波1分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//计数模式，向上计数
	//定时频率=72000000/(PSC+1)/(ARR+1),定时1毫秒
	TIM_TimeBaseInitStructure.TIM_Period=1000-1;//ARR,计数器，计数器加满触发定时
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;//PSC，预分频，预分频加满计数器加一
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数器，通用定时器无，给0
	TIM_TimeBaseInit (TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag (TIM2,TIM_FLAG_Update);//清除更新中断标志位，避免初始化之后立刻进入中断
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//配置中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;//配置通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd (TIM2,ENABLE);//启动定时器
}
/*
void TIM2_IRQHandler ()
{
	if (TIM_GetFlagStatus (TIM2,TIM_FLAG_Update)==SET)
	{
		
		TIM_ClearITPendingBit (TIM2,TIM_FLAG_Update);
	}
	
}
*/