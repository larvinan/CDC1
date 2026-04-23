#include "bsp_rgb.h"

// 用来保存当前RGB亮度值，方便判断亮灭状态
static uint8_t s_rgb_r = 0;
static uint8_t s_rgb_g = 0;
static uint8_t s_rgb_b = 0;

/*************************************************
 * 引脚定义 (共阴RGB)
 * PA8  -> TIM1_CH1  红
 * PB6  -> TIM4_CH1  绿
 * PA11 -> TIM1_CH4  蓝
 *************************************************/
static void TIM1_PWM_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;

    // 开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA8、PA11 复用推挽输出
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PWM周期 256，频率约3.9kHz
    TIM_TimeBaseStructure.TIM_Prescaler     = 72 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period        = 255;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // PWM模式1，高电平亮
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE); // 高级定时器必开
}

static void TIM4_PWM_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // PB6 复用推挽
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Prescaler     = 72 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period        = 255;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

// RGB总初始化
void RGB_Init(void)
{
    TIM1_PWM_Init();
    TIM4_PWM_Init();
    RGB_Off(); // 上电默认灭
}

// 设置颜色并保存当前值
void RGB_SetColor(uint8_t r, uint8_t g, uint8_t b)
{
    s_rgb_r = r;
    s_rgb_g = g;
    s_rgb_b = b;

    TIM_SetCompare1(TIM1, r);
    TIM_SetCompare1(TIM4, g);
    TIM_SetCompare4(TIM1, b);
}

/*************************************************
 * 函数：RGB_IsOn();
 * 功能：判断RGB是否亮着（只要不是全黑就返回1）
 * 返回：1=亮  0=灭
 *************************************************/
uint8_t RGB_IsOn(void)
{
    // 任意一个通道亮度>0，就认为灯亮
    if (s_rgb_r > 0 || s_rgb_g > 0 || s_rgb_b > 0)
    {
        return 1;
    }
    // 全部为0才是熄灭
    else 
		return 0;
}

// 红色
void RGB_Red(void)
{
    RGB_SetColor(255, 0, 0);
}

// 绿色
void RGB_Green(void)
{
    RGB_SetColor(0, 255, 0);
}

// 蓝色
void RGB_Blue(void)
{
    RGB_SetColor(0, 0, 255);
}

// 白色
void RGB_White(void)
{
    RGB_SetColor(255, 255, 255);
}

// 熄灭
void RGB_Off(void)
{
    RGB_SetColor(0, 0, 0);
}

/*************************************************
 * 函数：RGB_Rainbow_Task();
 * 功能：跳变彩虹（红→绿→蓝→黄→青→紫→白 循环跳变）
 * 使用：必须放在 while(1) 里不断调用
 *************************************************/
void RGB_Rainbow_Task(void)
{
    static uint8_t i = 0;

    switch(i)
    {
        case 0: RGB_SetColor(255, 0, 0);      break;
        case 1: RGB_SetColor(0, 255, 0);      break;
        case 2: RGB_SetColor(0, 0, 255);      break;
        case 3: RGB_SetColor(255, 255, 0);    break;
        case 4: RGB_SetColor(0, 255, 255);    break;
        case 5: RGB_SetColor(255, 0, 255);    break;
        case 6: RGB_SetColor(255, 255, 255);  break;
    }

    i++;
    if(i >= 7) i = 0;
}
