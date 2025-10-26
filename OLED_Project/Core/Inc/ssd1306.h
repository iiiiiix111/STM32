#ifndef __SSD1306_H
#define __SSD1306_H

#include "main.h"
#include "i2c.h"

// SSD1306 OLED配置
#define SSD1306_I2C_ADDR    0x3D  // 最常见的地址
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64

// 函数声明
uint8_t SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_Test_Pattern(void);
void SSD1306_Show_Message(char* line1, char* line2, char* line3, char* line4);

#endif