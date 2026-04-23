#ifndef __ESP8266_H__
#define __ESP8266_H__


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#define WIFI_NAME         "JAY"
#define WIFI_PASSWORD     "daddaddad"

void ESP8266_Init(void);
void MQTT_Init(void);
void MQTT_Publish_Data(float temperature,float humidity,uint8_t light,uint32_t occupancy,uint8_t flame,float smoke,uint8_t RGB);
//uint8_t MQTT_Get_Data(char *name,char *data1,char *data2,char *data3,char *data4,char *data5,char *data6);
uint8_t MQTT_Get_Data(char *name);
uint8_t ESP_WaitResp(char *wait_str,uint16_t timeout_ms);

extern uint8_t ESP8266_Init_Success;
extern uint8_t MQTT_Init_Success;

#endif
