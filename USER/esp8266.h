#ifndef __ESP8266_H__
#define __ESP8266_H__


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

// WiFi连接配置
#define WIFI_NAME         "JAY"         // WiFi网络名称
#define WIFI_PASSWORD     "daddaddad"   // WiFi密码

// 函数声明
void ESP8266_Init(void); // 初始化ESP8266模块
void MQTT_Init(void); // 初始化MQTT协议
void MQTT_Publish_Data(float temperature,float humidity,uint8_t light,uint32_t occupancy,uint8_t flame,float smoke,uint8_t RGB); // 发布传感器数据到MQTT服务器
//uint8_t MQTT_Get_Data(char *name,char *data1,char *data2,char *data3,char *data4,char *data5,char *data6); // 获取MQTT数据（已注释）
uint8_t MQTT_Get_Data(char *name); // 获取指定名称的MQTT数据
uint8_t ESP_WaitResp(char *wait_str,uint16_t timeout_ms); // 等待ESP8266响应
uint8_t ESP_WaitResp(char *wait_str,uint16_t timeout_ms); // 等待ESP8266响应（重复声明？）

// 外部变量声明
extern uint8_t ESP8266_Init_Success; // ESP8266初始化成功标志
extern uint8_t MQTT_Init_Success; // MQTT初始化成功标志

#endif


