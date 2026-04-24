/*项目工作日志
2026-4-15 20:30 Created by LWH
2026-4-16 02:40 Updated by LWH 调试温湿度DHT11和烟雾传感器MQ135驱动代码 添加可移植ADC封装函数 
OLED实时显示当前温湿度和气体浓度 超过设定阈值时触发文字警报
2026-4-16 21:08 Updated by LWH 重新整理温湿度和气体检测模块代码 矫正烟雾阈值
2026-4-17 18:02 Updated by LWH 添加蜂鸣器驱动 空气质量过低会出触发蜂鸣器警报
2026-4-17 23:45 Updated by LWH 添加esp8266 usart1、2串口 onenet相关代码和驱动
2026-4-19 02:07 UPdated by LWH 添加光敏传感器驱动封装(By LSY) 调试光照阈值 oled显示实时状态(bright/Dark)
2026-4-19 04:54 Updated by LWH 添加火焰传感器驱动封装 将检测的火焰状态在oled上显示(1 SAFE/0 FIRE) 
有火焰时触发蜂鸣器警报
2026-4-19 21:30 Updated by LWH 添加MQTT相关代码 添加MQTT发布数据到云端 发送当前传感器数据到云端onenet 平台
2026-4-20 23:15 Updated by LWH 添加RGB驱动封装模拟教室灯光 添加MQTT订阅云端RGB控制命令 根据命令切换RGB状态(关闭/白/红/绿/蓝/彩虹动画)
2026-4-21 02:30 Updated by LWH 整理主循环代码结构 优化显示逻辑和MQTT交互逻辑
2026-4-24 20:00 Updated by LWH 添加注释说明代码逻辑和功能 上传源码到GitHub
2026-4-25 04:05 Updated by LWH 导入工程文件到vscode进行调试 编译和stlink下载
*/

#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bsp_DHT11.h"
#include "adc.h"
#include "MQ135.h"
#include "BUZZER.h"
#include "stm32f10x.h"
#include "ldr.h"
#include "flame.h"
#include "esp8266.h"
#include "Serial.h"
#include "bsp_rgb.h"
#include "red_temp_monitor.h"

DHT11_Data_TypeDef DHT11_Data;
float air_quality;
uint8_t RGB_data;
uint8_t occ = 1;

int main(void)
{
    /* ------------------ 硬件与外设初始化 ------------------ */
    /* 蜂鸣器初始化：用于告警提示 */
    Beep_Init();
    /* 延时/滴答初始化，提供 delay_ms 等函数 */
    delay_init();
    /* OLED 初始化并清屏，用于显示传感器数据 */
    OLED_Init();
    OLED_Clear();
    /* RGB 灯条初始化 */
    RGB_Init();

    /* 传感器初始化：按需初始化各传感器/ADC/通讯模块 */
    LDR_Init();        /* 光敏传感器 */
    DHT11_Init();      /* 温湿度传感器 */
    ADC_myInit();      /* ADC（可能用于读取模拟传感器） */
    MQ135_Init();      /* 空气质量传感器 */
    Flame_Init();      /* 火焰传感器 */
    ESP8266_Init();    /* ESP8266 WiFi 模块 */

    /* 如果 ESP8266 初始化成功，则初始化 MQTT（或其它云端功能） */
    if (ESP8266_Init_Success)
    {
        MQTT_Init();
    }

    /* ------------------ 主循环：采集 -> 显示 -> 发布 -> 灯控 ------------------ */
    while (1)
    {
        OLED_Clear();

        /* 仅在 MQTT/云端功能就绪时进行传感器读取与上报 */
        if (MQTT_Init_Success)
        {
            /*
             * 读取温湿度与空气质量：
             * - DHT11_Read_TempAndHumidity 返回是否读取成功
             * - MQ135_Get_Air_Quality 返回空气质量数值（0~100 或其它量纲）
             */
            if (DHT11_Read_TempAndHumidity(&DHT11_Data) && MQ135_Get_Air_Quality())
            {
                air_quality = MQ135_Get_Air_Quality();

                /* 在 OLED 上显示采集到的数据，坐标与字体取决于驱动实现 */
                OLED_ShowString(0, 35, "AQ: ", OLED_8X16); /* 空气质量 */
                OLED_ShowString(0, 0, "light: ", OLED_8X16);
                OLED_ShowNum(45, 0, LDR_ReadLight(), 3, OLED_8X16); /* 光照强度 */
                OLED_ShowString(0, 20, "flame: ", OLED_8X16);
                OLED_ShowNum(45, 20, Flame_IsDetected(), 1, OLED_8X16); /* 火焰传感器原始状态 */
                OLED_ShowNum(20, 35, air_quality, 2, OLED_8X16);
                OLED_ShowString(0, 50, "Tem: ", OLED_8X16);
                OLED_ShowNum(30, 50, DHT11_Data.temperature, 2, OLED_8X16);
                OLED_ShowString(60, 50, "Hum: ", OLED_8X16);
                OLED_ShowNum(90, 50, DHT11_Data.humidity, 2, OLED_8X16);
                OLED_ShowString(110, 50, "%", OLED_8X16);

                /* 简单阈值判断示例：空气质量过差时触发告警 */
                if (air_quality < 25)
                {
                    /* air_quality 数值越小/越大代表意义由 MQ135 驱动定义 */
                    OLED_ShowString(50, 35, "Emergency", OLED_8X16);
                    Beep_Start(); /* 蜂鸣器告警 */
                }
                else
                {
                    OLED_ShowString(60, 35, "Clean", OLED_8X16);
                    Beep_Stop();
                }

                /* 光照判断：阈值 45 为示例，可根据实际传感器校准 */
                if (LDR_ReadLight() >= 45)
                {
                    OLED_ShowString(80, 0, "bright", OLED_8X16);
                }
                else
                {
                    OLED_ShowString(80, 0, "Dark", OLED_8X16);
                }

                /*
                 * 火焰检测：注意函数返回值语义
                 * 当前代码中：Flame_IsDetected() == 0 表示检测到火焰（基于原实现）
                 */
                if (Flame_IsDetected() == 0)
                {
                    Beep_Start();
                    OLED_ShowString(60, 20, "FIRE", OLED_8X16);
                }
                else
                {
                    Beep_Stop();
                    OLED_ShowString(60, 20, "SAFE", OLED_8X16);
                }

                OLED_Update(); /* 将缓冲区内容刷新到屏幕 */
            }

            /* 获取当前 RGB 状态并将所有关键数据通过 MQTT 发布到云端 */
            RGB_data = RGB_IsOn();
            MQTT_Publish_Data(DHT11_Data.temperature,
                              DHT11_Data.humidity,
                              LDR_ReadLight(),
                              0,
                              Flame_IsDetected(),
                              MQ135_Get_Air_Quality(),
                              RGB_data);
        }

        /*
         * 从 MQTT/云端读取是否有对 RGB 的控制命令
         * - MQTT_Get_Data 返回 0xFF 表示无新命令
         * - 否则返回 mode 值，根据值切换 RGB 模式
         */
        uint8_t mode = MQTT_Get_Data("\"RGB\"");
        if (mode != 0xFF)
        {
            switch (mode)
            {
                case 0: /* 关闭 */
                    RGB_Off();
                    break;
                case 1: /* 白色 */
                    RGB_White();
                    break;
                case 2: /* 红色 */
                    RGB_Red();
                    break;
                case 3: /* 绿色 */
                    RGB_Green();
                    break;
                case 4: /* 蓝色 */
                    RGB_Blue();
                    break;
                case 5: /* 彩虹/动画任务（可能为非阻塞任务） */
                    RGB_Rainbow_Task();
                    break;
            }
        }

        /* 主循环延时，控制采样/上报频率（2 秒） */
        delay_ms(2000);
    }
}




