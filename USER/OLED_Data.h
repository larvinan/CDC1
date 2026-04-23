#ifndef __OLED_DATA_H
#define __OLED_DATA_H

#include <stdint.h>

/*中文字符字节宽度*/
#define OLED_CHN_CHAR_WIDTH			2		//UTF-8编码格式给3，GB2312编码格式给2

/*字模基本单元*/
typedef struct 
{
	char Index[OLED_CHN_CHAR_WIDTH + 1];	//汉字索引
	uint8_t Data[32];						//字模数据
} ChineseCell_t_16X16;
typedef struct 
{
	char Index[OLED_CHN_CHAR_WIDTH + 1];	//汉字索引
	uint8_t Data[24];						//字模数据
} ChineseCell_t_12X12;

/*ASCII字模数据声明*/
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];
extern const uint8_t OLED_F12x24[][36];
/*汉字字模数据声明*/
extern const ChineseCell_t_16X16 OLED_CF16x16[];
extern const ChineseCell_t_12X12 OLED_CF12x12[];
/*图像数据声明*/
extern const uint8_t Diode[];
extern const uint8_t Cursor[];
extern const uint8_t Climate[];
extern const uint8_t Set[];
extern const uint8_t Connect[];
extern const uint8_t Light[];
extern const uint8_t SOS[];
extern const uint8_t Battery[];
extern const uint8_t duo[];
extern const uint8_t mode[];
/*按照上面的格式，在这个位置加入新的图像数据声明*/
//...

#endif


/*****************江协科技|版权所有****************/
/*****************jiangxiekeji.com*****************/
