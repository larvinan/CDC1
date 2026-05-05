#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "delay.h"
#include "OLED.h"
#include "esp8266.h"
#include <string.h>

uint8_t ESP8266_Init_Success=0; // ESP8266 初始化成功标志（esp-01s）
uint8_t MQTT_Init_Success=0;    // MQTT 初始化成功标志

/* ESP8266 初始化函数
 *  作用：初始化串口、检测模块、重启模块、设置工作模式并连接 WiFi
 */
void ESP8266_Init()
{
	Serial_Init(1);
	Serial_Init(2);
	//OLED_Init();
	delay_ms(500);
	
	/* 1: 发送 AT 命令测试模块是否响应 */
	for (uint8_t i=0;i<=1;i++) // 发送两次 AT 以确保模块响应
	{
		USART2_RX_LEN=0;
		memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
		Serial_SendString(2,"AT\r\n");
		Serial_SendString(1,"AT测试\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"AT",OLED_8X16);
		OLED_Update();
		delay_ms(1000);
	}
	
	if (ESP_WaitResp("OK",6000))
	{
		Serial_SendString(1,"AT测试成功\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"OK",OLED_8X16);
		OLED_Update();
	}
	else
	{
		Serial_SendString(1,"AT测试失败\r\n");
		OLED_ShowString(0,0,"ERROR",OLED_8X16);
		OLED_Clear();
		OLED_Update();
		delay_ms(500);
		return;
	}
	
	/* 2: 重启模块（软件复位） */
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,"AT+RST\r\n");
	Serial_SendString(1,"复位\r\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+RST",OLED_8X16);
	OLED_Update();
	delay_ms(1000);
	
	/* 3: 设置为 STA（Station）模式 */
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,"AT+CWMODE=1\r\n");
	Serial_SendString(1,"设置为sta模式\r\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+CWMODE=1",OLED_8X16);
	OLED_Update();
	delay_ms(300);
	
	/* 4: 连接到指定的 WiFi（SSID / PASSWORD 在宏中定义） */
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,"AT+CWJAP=\""WIFI_NAME"\",\""WIFI_PASSWORD"\"\r\n");
	Serial_SendString(1,"连接网络\r\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+CWJAP",OLED_8X16);
	OLED_Update();
	
	uint8_t conn_flag=0;
	uint8_t ip_flag=0;
	uint32_t start_time=0;
	
	while (start_time<10000) // 等待最多 10 秒，检测连接和获取 IP 的提示
	{
		if (strstr(USART2_RX_BUF,"WIFI CONNECTED")!=NULL) conn_flag=1;
		if (strstr(USART2_RX_BUF,"WIFI GOT IP")!=NULL) ip_flag=1;
		if (conn_flag && ip_flag) break; // 同时获得连接和 IP 则退出
		delay_ms(10);
		start_time+=10;
	}
	//Serial_SendString(1,USART2_RX_BUF);
	if (conn_flag && ip_flag)
	{
		Serial_SendString(1,"网络连接成功\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"OK",OLED_8X16);
		OLED_Update();
	}
	else
	{
		Serial_SendString(1,"网络连接失败\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"ERROR",OLED_8X16);
		OLED_Update();
		delay_ms(500);
		return;
	}
	
	// 标记初始化成功
	ESP8266_Init_Success=1;
	
	delay_ms(2000);
}

/* MQTT 初始化函数
 *  作用：配置 MQTT 用户信息，连接 OneNET 服务，并订阅需要的主题
 */
void MQTT_Init()
{
	/* 1: 设置 MQTT 用户配置（设备 ID、用户名、签名等） */
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,
	"AT+MQTTUSERCFG=0,1,\"demo2\",\"Humkm2F82M\",\"version=2018-10-31&res=products%2FHumkm2F82M%2Fdevices%2Fdemo2&et=1805693871&method=md5&sign=H3s0PrwjYVLKgT3UF5hk0A%3D%3D\",0,0,\"\"\r\n");
	Serial_SendString(1,"设置用户属性\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+MQTTUSERCFG",OLED_8X16);
	OLED_Update();
	
	if (ESP_WaitResp("OK",6000))
	{
		Serial_SendString(1,"设置用户属性成功\n");
		OLED_Clear();
		OLED_ShowString(0,0,"OK",OLED_8X16);
		OLED_Update();
	}
	else
	{
		Serial_SendString(1,"设置用户属性失败\n");
		OLED_ShowString(0,0,"ERROR",OLED_8X16);
		OLED_Clear();
		OLED_Update();
		delay_ms(500);
		return;
	}
	
	/* 2: 连接 OneNET 服务 */
	delay_ms(3000);
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,"AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
	Serial_SendString(1,"连接onenet\r\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+MQTTCONN",OLED_8X16);
	OLED_Update();
	
	if (ESP_WaitResp("OK",8000))
	{
		Serial_SendString(1,"OneNET连接成功\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"OK",OLED_8X16);
		OLED_Update();
	}
	else
	{
		Serial_SendString(1,"OneNET连接失败\r\n");
		OLED_ShowString(0,0,"ERROR",OLED_8X16);
		OLED_Clear();
		OLED_Update();
		delay_ms(500);
		return;
	}
	
	/* 3: 订阅需要的主题（上报回复、属性设置等） */
	delay_ms(1000);
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,"AT+MQTTSUB=0,\"$sys/Humkm2F82M/demo2/thing/property/post/reply\",1\r\n");//lwh 0->1
	Serial_SendString(1,"订阅“设备属性上报响应”主题\r\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+MQTTSUB:reply",OLED_8X16);
	OLED_Update();
	
	if (ESP_WaitResp("OK",6000))
	{
		Serial_SendString(1,"订阅成功\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"OK",OLED_8X16);
		OLED_Update();
	}
	else
	{
		Serial_SendString(1,"订阅失败\r\n");
		OLED_ShowString(0,0,"ERROR",OLED_8X16);
		OLED_Clear();
		OLED_Update();
		delay_ms(500);
		return;
	}
	
	delay_ms(1000);
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,"AT+MQTTSUB=0,\"$sys/Humkm2F82M/demo2/thing/property/set\",0\r\n");
	Serial_SendString(1,"订阅“设备属性设置请求”主题\r\n");
	OLED_Clear();
	OLED_ShowString(0,0,"AT+MQTTSUB:set",OLED_8X16);
	OLED_Update();
	
	if (ESP_WaitResp("OK",6000))
	{
		Serial_SendString(1,"订阅成功\r\n");
		OLED_Clear();
		OLED_ShowString(0,0,"OK",OLED_8X16);
		OLED_Update();
	}
	else
	{
		Serial_SendString(1,"订阅失败\r\n");
		OLED_ShowString(0,0,"ERROR",OLED_8X16);
		OLED_Clear();
		OLED_Update();
		delay_ms(500);
		return;
	}
	
	MQTT_Init_Success=1;
	delay_ms(2000);
}

/*涓婁紶鏁版嵁鍑芥暟
*/
void MQTT_Publish_Data(float temperature,float humidity,uint8_t light,uint32_t occupancy,uint8_t flame,float smoke,uint8_t RGB)
{
	char data[256];
	snprintf(data,sizeof(data),"{\"id\":\"123\",\"params\":{"
    "\"temperature\":{\"value\":%.2f},"
    "\"humidity\":{\"value\":%.2f},"
    "\"light\":{\"value\":%d},"
    "\"occupancy\":{\"value\":%d},"
    "\"flame\":{\"value\":%d},"
    "\"smoke\":{\"value\":%.2f},"
	  "\"RGB\":{\"value\":%u}"
    "}}",
    temperature, humidity, light, occupancy, flame, smoke, RGB);
	
	uint16_t data_len=strlen(data); // 计算数据长度（不包括字符串终止符）
	
	char at_cmd[128];
	snprintf(at_cmd,sizeof(at_cmd),"AT+MQTTPUBRAW=0,\"$sys/Humkm2F82M/demo2/thing/property/post\",%d,0,0\r\n",data_len);
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,at_cmd);
	Serial_SendString(1,"准备发送数据\r\n");
	//OLED_Clear();
	//OLED_ShowString(0,0,"AT+MQTTPUBRAW",OLED_8X16);
	//OLED_Update();
	
	if (ESP_WaitResp(">",6000))
	{
		Serial_SendString(1,"准备发送数据成功\r\n");
		//OLED_Clear();
		//OLED_ShowString(0,0,"OK",OLED_8X16);
		//OLED_Update();
	}
	else
	{
		Serial_SendString(1,"准备发送数据失败\r\n");
		//OLED_ShowString(0,0,"ERROR",OLED_8X16);
		//OLED_Clear();
		//OLED_Update();
		delay_ms(500);
		return;
	}
	
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
	Serial_SendString(2,data);
	Serial_SendString(1,"在正在发送数据\r\n");
	//OLED_Clear();
	//OLED_ShowString(0,0,"send...",OLED_8X16);
	//OLED_Update();
	
	if (ESP_WaitResp("OK",6000))
	{
		Serial_SendString(1,"发送数据成功\r\n");
		//OLED_Clear();
		//OLED_ShowString(0,0,"OK",OLED_8X16);
		//OLED_Update();
	}
	else
	{
		Serial_SendString(1,"发送数据失败\r\n");
		//OLED_ShowString(0,0,"ERROR",OLED_8X16);
		//OLED_Clear();
		//OLED_Update();
		delay_ms(500);
		return;
	}
	
	USART2_RX_LEN=0;
	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
}

/*鎺ユ敹鏁版嵁鍑芥暟

uint8_t MQTT_Get_Data(char *name,char *data1,char *data2,char *data3,char *data4,char *data5,char *data6)
{
	if (strstr(USART2_RX_BUF,name)!=NULL)//鎺ユ敹鍒板甫鏈夋爣璇嗙鐨勬暟鎹?
	{
		uint8_t data=0;
		Serial_SendString(1,"鎴愬姛鎺ユ敹鍒版暟鎹甛r\n");
		Serial_SendString(1,USART2_RX_BUF);
		if (strstr(USART2_RX_BUF,data1)!=NULL)
		{
			data=1;
		}
		else if (strstr(USART2_RX_BUF,data2)!=NULL)
		{
			data=2;
		}
		else
		{
			data=0;
		}
		if (data!=0)
		{
			USART2_RX_LEN=0;
			memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
		}
		*/
		
/*接收数据函数
*/
uint8_t MQTT_Get_Data(char *name,char *data1,char *data2)
{
	if (strstr(USART2_RX_BUF,name)!=NULL)//接收到带有标识符的数据
	{
		uint8_t data=0;
		Serial_SendString(1,"成功接收到数据\r\n");
		Serial_SendString(1,USART2_RX_BUF);
		if (strstr(USART2_RX_BUF,data1)!=NULL)//提取到指令1
		{
			data=1;
		}
		else if (strstr(USART2_RX_BUF,data2)!=NULL)//提取到指令2
		{
			data=2;
		}
		else
		{
			data=0;
		}
		if (data!=0)//提取到指令后，清空数组，防止对下次判断产生影响
		{
			USART2_RX_LEN=0;
			memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
		}
		
		return data;
	}
	else
	{
		return 0;
	}
}


uint8_t ESP_WaitResp(char *wait_str,uint16_t timeout_ms)
{
	uint32_t start=0;

	while (start<timeout_ms)
	{
		if (strstr(USART2_RX_BUF,wait_str)!=NULL)
		{
			return 1; // 找到返回 1
		}
		delay_ms(1);
		start++;
	}
	return 0; // 超时返回 0
}

