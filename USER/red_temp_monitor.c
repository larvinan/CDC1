#include "red_temp_monitor.h"
#include "delay.h" // 你的延时函数

/**
 * @brief  红外模块GPIO初始化
 * @note   发射脚推挽输出，一直发射；接收脚上拉输入
 */
void IR_Obstacle_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启GPIO时钟
    RCC_APB2PeriphClockCmd(IR_TX_CLK | IR_RX_CLK, ENABLE);

    // 1. 红外发射模块（HW-489）初始化：推挽输出，持续发射
    GPIO_InitStructure.GPIO_Pin   = IR_TX_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_TX_PORT, &GPIO_InitStructure);

    // 持续发射红外光（拉高，让LED常亮）
    GPIO_SetBits(IR_TX_PORT, IR_TX_PIN);

    // 2. 红外接收模块（HW-490）初始化：上拉输入
    GPIO_InitStructure.GPIO_Pin   = IR_RX_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(IR_RX_PORT, &GPIO_InitStructure);
}

/**
 * @brief  读取红外接收模块原始电平
 * @retval 0=接收到红外光（无遮挡），1=未接收到红外光（被遮挡）
 */
static uint8_t IR_Receive_Raw(void)
{
    return GPIO_ReadInputDataBit(IR_RX_PORT, IR_RX_PIN);
}

/**
 * @brief  带消抖的红外遮挡检测
 * @retval 1=检测到人员（光路被挡住），0=无人员
 */
uint8_t IR_Detect_Person(void)
{
    // 第一次检测到遮挡信号
    if(IR_Receive_Raw() == 1)
    {
        delay_ms(IR_DEBOUNCE_MS); // 软件消抖，防止误触发
        // 再次确认信号
        if(IR_Receive_Raw() == 1)
        {
            return 1; // 确认有人挡住了红外光路
        }
    }
    return 0; // 无遮挡，未检测到人员
}


