#ifndef __OLED_TEST_H
#define __OLED_TEST_H

#include "main.h"
#include "i2c.h"
#include "usart.h"

#define OLED_ADDR 0x78  // OLED I2C地址，也可能是0x7A

// 函数声明
void OLED_Test_Init(void);
void OLED_Test_Clear(void);
void OLED_Test_ShowStatus(char *status);
void OLED_Test_ShowText(uint8_t line, char *text);
void OLED_Test_AllFunctions(void);

#endif