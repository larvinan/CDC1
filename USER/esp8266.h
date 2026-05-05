#ifndef __ESP8266_H__
#define __ESP8266_H__


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

// WiFi
#define WIFI_NAME         "JAY"         // WiFi
#define WIFI_PASSWORD     "daddaddad"   // WiFi

// 
void ESP8266_Init(void); // 初始化 ESP8266 模块
void MQTT_Init(void); // 初始化 MQTT 客户端
uint8_t MQTT_Get_Data(char *name,char *data1,char *data2);
void MQTT_Publish_Data(float temperature,float humidity,uint8_t light,uint32_t occupancy,uint8_t flame,float smoke,uint8_t RGB); // 发布 MQTT 数据
//uint8_t MQTT_Get_Data(char *name,char *data1,char *data2,char *data3,char *data4,char *data5,char *data6); // 接收 MQTT 数据函数声明
//uint8_t MQTT_Get_Data(char *name); // 订阅/接收 MQTT
uint8_t ESP_WaitResp(char *wait_str,uint16_t timeout_ms); // 等待 ESP8266 返回
void ESP_ClearRxBuf(void); // 清除接收缓冲区
// ESP 状态机 API
// 每 1ms 由 SysTick 调用
void ESP_MillisTick(void);
// ESP/MQTT 状态机处理
void ESP_Process(void);
// 启动 MQTT 服务
void ESP_StartMQTTService(void);
// 请求 ESP_Process 发布数据
void ESP_RequestPublish_NB(float temperature,float humidity,uint8_t light,uint32_t occupancy,uint8_t flame,float smoke,uint8_t RGB);

// 状态/事件枚举定义
typedef enum {
	ESP_STATE_IDLE = 0,
	ESP_STATE_MQTT_USERCFG,
	ESP_STATE_MQTT_CONN,
	ESP_STATE_MQTT_SUB1,
	ESP_STATE_MQTT_SUB2,
	ESP_STATE_CONNECTED,
	ESP_STATE_PUBLISH_PREPARE,
	ESP_STATE_PUBLISH_WAIT_PROMPT,
	ESP_STATE_PUBLISH_WAIT_OK,
	ESP_STATE_RECONNECT_WAIT,
} ESP_State_t;

// 获取当前状态
ESP_State_t ESP_GetState(void);

// 标志
extern uint8_t ESP8266_Init_Success; // ESP8266 初始化成功
extern uint8_t MQTT_Init_Success; // MQTT 初始化成功

#endif
