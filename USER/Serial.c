#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

char USART1_RX_BUF[1024]={0};
uint16_t USART1_RX_LEN=0;
uint8_t USART1_RX_FINISH=0;

char USART2_RX_BUF[1024]={0};
uint16_t USART2_RX_LEN=0;
uint8_t USART2_RX_FINISH=0;

void Serial_Init (uint8_t USART_Name)
{
	if (USART_Name==1)
	{
		RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1,ENABLE);//开启USART1的时钟
		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA,ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate=115200;//波特率
		USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//不使用流控
		USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//发送+接收模式
		USART_InitStructure.USART_Parity=USART_Parity_No;//校验位，无校验
		USART_InitStructure.USART_StopBits=USART_StopBits_1;//停止位，1位
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;//字长
		USART_Init (USART1,&USART_InitStructure);
		
		USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//开启RXNE标志位到NVIC的输出,RXNE标志位一旦为1,就申请中断
		
		NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;//中断通道
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
		NVIC_Init (&NVIC_InitStructure);
		
		USART_Cmd (USART1,ENABLE);
	}
	else if (USART_Name==2)
	{
		RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2,ENABLE);//开启USART2的时钟
		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA,ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate=115200;//波特率
		USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//不使用流控
		USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//发送+接收模式
		USART_InitStructure.USART_Parity=USART_Parity_No;//校验位，无校验
		USART_InitStructure.USART_StopBits=USART_StopBits_1;//停止位，1位
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;//字长
		USART_Init (USART2,&USART_InitStructure);
		
		USART_ITConfig (USART2,USART_IT_RXNE,ENABLE);//开启RXNE标志位到NVIC的输出,RXNE标志位一旦为1,就申请中断
		
		NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;//中断通道
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
		NVIC_Init (&NVIC_InitStructure);
		
		USART_Cmd (USART2,ENABLE);
	}
}

void Serial_SendByte (uint8_t USART_Name,uint8_t Byte)//发送一个字节
{
	if (USART_Name==1)
	{
		USART_SendData (USART1,Byte);
		while (USART_GetFlagStatus (USART1,USART_FLAG_TXE) == RESET);//判断标志位，可自动清零
	}
	else if (USART_Name==2)
	{
		USART_SendData (USART2,Byte);
		while (USART_GetFlagStatus (USART2,USART_FLAG_TXE) == RESET);//判断标志位，可自动清零
	}
	
}

void Serial_SendArray (uint8_t USART_Name,uint8_t *Array,uint16_t Length)//发送一个数组
{
	uint16_t i;
	for (i=0;i<Length;i++)
	{
		Serial_SendByte (USART_Name,Array[i]);
	}
}
void Serial_SendString (uint8_t USART_Name,char *String)//发送字符串
{
	uint16_t i;
	for (i=0;String[i]!='\0';i++)
	{
		Serial_SendByte (USART_Name,String[i]);
	}
}
uint32_t Serial_Pow (uint32_t x,uint32_t y)
{
	uint32_t Resuit =1;
	while (y--)
	{
		Resuit*=x;
	}
	return Resuit;
}
void Serial_SendNumber (uint8_t USART_Name,uint32_t Number,uint16_t Length)//发送一个数字
{
	uint16_t i;
	for (i=0;i<Length;i++)
	{
		Serial_SendByte (USART_Name,Number/Serial_Pow(10,Length-i-1)%10+0x30);
	}
}

void Serial_Printf (uint8_t USART_Name,char *format,...)//Printf函数
{
	char String [100];
	va_list arg;
	va_start (arg,format);
	vsprintf (String,format,arg);
	va_end (arg);
	Serial_SendString (USART_Name,String);
}

void USART1_IRQHandler(void)
{
	uint8_t temp;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(USART1);
		USART1_RX_BUF[USART1_RX_LEN++] = temp;
		
		// 缓冲区防溢出
		if (USART1_RX_LEN >= 1023) USART1_RX_LEN = 0;
		
		// 检测到指令结束符\n（对应\r\n），标记接收完成
		if(temp == '\n') 
		{
			USART1_RX_FINISH = 1;
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART2_IRQHandler(void)
{
	uint8_t temp;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(USART2);//读取接收字节
		USART2_RX_BUF[USART2_RX_LEN++] = temp;
		
		// 缓冲区防溢出
		if (USART2_RX_LEN >= 1023) USART2_RX_LEN = 0;
		
		// 检测到指令结束符\n（对应\r\n），标记接收完成
		if(temp == '\n') 
		{
			USART2_RX_FINISH = 1;
		}
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}



