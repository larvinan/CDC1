#include "sensor_led.h"

/************************************************
函数名称：Sensor_Led_Init
功    能：初始化 PB3(人体感应)
************************************************/
void Sensor_Led_Init(void)
{
    // 开 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    //==================== PB3 人体感应输入 ====================
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/************************************************
函数名称：Human_Read
功    能：读取人体状态
返    回：1=有人  0=无人
************************************************/
uint8_t Human_Read(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)
        return 1;
    else
        return 0;
}


