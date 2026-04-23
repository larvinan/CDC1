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
2026-4-19 23:55 updated by LWH  完善esp8266和oled驱动代码 优化函数调用
2026-4-20 02:14 updated by LWH  添加serial串口函数 测试 esp8266连接WiFi 连接onenet网站
2026-4-21 04:11 updated by LWH  成功调试并实现实时数据上传 温湿度 烟雾 光照强度和火焰检测实时显示在onenet平台
2026-4-21 23:40 updated by LWH 添加RGB驱动封装 RGB模拟教室灯光 计划实现云端下发数据实现开关灯
2026-4-
*/
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bsp_DHT11.h"
#include "adc.h"
#include "MQ135.h"
#include "BUZZER.h"         
#include "stm32f10x.h"   								// Device header
#include "ldr.h"
#include "flame.h"
#include "esp8266.h"
#include "Serial.h"
#include "bsp_rgb.h"
#include "red_temp_monitor.h"
DHT11_Data_TypeDef DHT11_Data;//存放温度，湿度值
float air_quality;
uint8_t RGB_data;
uint8_t occ=1;
//主函数，采用外部8M晶振，72M系统主频，可以在void SetSysClock(void)函数中选择主频率设置
int main(void)
 {	
	Beep_Init();
	delay_init();	//延时函数初始化,通过Systick中断实现1ms延时功能  
	OLED_Init();	//初始化OLED  
	OLED_Clear(); //清屏
	RGB_Init();
	
	 
  LDR_Init();            
	DHT11_Init(); //DHT11初始化
	ADC_myInit();
	MQ135_Init();
  Flame_Init();
  ESP8266_Init();
	if(ESP8266_Init_Success)
	{
		 MQTT_Init();
		 //MQTT_Publish_Data(25,56,78,0,1,55	);
	}
	 
	 
	while(1) 
	{	
		//RGB_Red();
		//RGB_Green();
   //RGB_Blue();
//			RGB_White();
    //RGB_Off();
		//RGB_Rainbow_Task();		
		OLED_Clear();
		
		if(MQTT_Init_Success)
	{
		if(DHT11_Read_TempAndHumidity(&DHT11_Data)&&MQ135_Get_Air_Quality())//读取温度，湿度
		{	
		air_quality =MQ135_Get_Air_Quality();
		//OLED_Clear();
		OLED_ShowString(0, 35, "AQ: ",OLED_8X16);
		OLED_ShowString(0, 0, "light: ",OLED_8X16);
		OLED_ShowNum(45,0,LDR_ReadLight(),3,OLED_8X16);
		OLED_ShowString(0, 20, "flame: ",OLED_8X16);
		OLED_ShowNum(45, 20, Flame_IsDetected(),1,OLED_8X16);
    OLED_ShowNum(20, 35, air_quality,2,OLED_8X16);
    //OLED_ShowString(60, 2, "/100");		
		OLED_ShowString(0,50,"Tem: ",OLED_8X16);  
		OLED_ShowNum(30,50,DHT11_Data.temperature,2,OLED_8X16);//显示温度，单位度 
		OLED_ShowString(60,50,"Hum: ",OLED_8X16);  
		OLED_ShowNum(90,50,DHT11_Data.humidity,2,OLED_8X16);//显示湿度
		OLED_ShowString(110,50,"%",OLED_8X16);
			if(air_quality<25) 
			{
			OLED_ShowString(50, 35, "Emergency",OLED_8X16);
			Beep_Start();
			}
			if(air_quality>=25)
			{
				OLED_ShowString(60, 35, "Clean",OLED_8X16);
				Beep_Stop();
			}
			if(LDR_ReadLight()>=45)
			{
				OLED_ShowString(80, 0, "bright",OLED_8X16);		
			}
			if(LDR_ReadLight()<45)
			{
				
				OLED_ShowString(80, 0, "Dark",OLED_8X16);	
		//if(occ==1){RGB_White();}
			}
			if(Flame_IsDetected()==0) 
			{
			Beep_Start();
			OLED_ShowString(60, 20, "FIRE",OLED_8X16);
			}
			if(Flame_IsDetected()==1)
			{
				Beep_Stop();
				OLED_ShowString(60, 20, "SAFE",OLED_8X16);
			}
			
			OLED_Update();
		}
			RGB_data=RGB_IsOn();   
			MQTT_Publish_Data(DHT11_Data.temperature,DHT11_Data.humidity,LDR_ReadLight(),0,Flame_IsDetected(),MQ135_Get_Air_Quality(),RGB_data);
	}
		uint8_t mode = MQTT_Get_Data("\"RGB\"");
		if(mode != 0xFF)  // 收到有效指令
	{
    switch(mode)
    {
        case 0:	RGB_Off();	 				break;
        case 1: RGB_White(); 				break;
        case 2: RGB_Red();	 				break;
        case 3: RGB_Green(); 				break;
        case 4: RGB_Blue();				  break;
        case 5: RGB_Rainbow_Task(); break;
    }
	}
		delay_ms(2000);//等待2s
	}	  
	
 }

