#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c.h"

#define OLED_ADDR 0x78

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowTempHumi(float temp, float humi);

#endif