#ifndef __AHT20_H
#define __AHT20_H

#include "main.h"
#include "i2c.h"

// AHT20 I2C地址
#define AHT20_ADDRESS_WRITE  (0x38 << 1)      // 0x70
#define AHT20_ADDRESS_READ   ((0x38 << 1) | 1) // 0x71

// AHT20命令定义
#define AHT20_CMD_INIT       0xBE    // 初始化命令
#define AHT20_CMD_MEASURE    0xAC    // 触发测量命令
#define AHT20_CMD_SOFTRESET  0xBA    // 软复位命令
#define AHT20_CMD_STATUS     0x71    // 状态查询

// 状态字位定义
#define AHT20_STATUS_BUSY    0x80    // 忙标志位
#define AHT20_STATUS_CALIB   0x08    // 校准使能位
#define AHT20_STATUS_CMD     0x01    // 命令模式位

// 函数声明
uint8_t AHT20_Init(void);
uint8_t AHT20_SoftReset(void);
uint8_t AHT20_Read_Status(void);
uint8_t AHT20_Start_Measurement(void);
uint8_t AHT20_Read_Data(float *temperature, float *humidity);
void AHT20_Read_Process(float *temp, float *humi);

#endif