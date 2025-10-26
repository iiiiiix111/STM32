#include "aht20.h"
#include <string.h>
#include<stdio.h>

// AHT20软复位
uint8_t AHT20_SoftReset(void)
{
    uint8_t cmd = AHT20_CMD_SOFTRESET;
    
    if(HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS_WRITE, &cmd, 1, 100) != HAL_OK)
    {
        return 1;
    }
    
    HAL_Delay(20);
    return 0;
}

// AHT20初始化
uint8_t AHT20_Init(void)
{
    uint8_t cmd[3] = {AHT20_CMD_INIT, 0x08, 0x00};
    
    // 先尝试软复位
    AHT20_SoftReset();
    HAL_Delay(10);
    
    // 发送初始化命令
    if(HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS_WRITE, cmd, 3, 100) != HAL_OK)
    {
        return 1;
    }
    
    HAL_Delay(10);
    
    // 检查校准状态
    uint8_t status = AHT20_Read_Status();
    if((status & 0x68) != 0x08)
    {
        return 2;
    }
    
    return 0;
}

// 读取状态字
uint8_t AHT20_Read_Status(void)
{
    uint8_t status = 0;
    HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS_READ, &status, 1, 100);
    return status;
}

// 开始测量
uint8_t AHT20_Start_Measurement(void)
{
    uint8_t cmd[3] = {AHT20_CMD_MEASURE, 0x33, 0x00};
    
    if(HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS_WRITE, cmd, 3, 100) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

// 读取温湿度数据
uint8_t AHT20_Read_Data(float *temperature, float *humidity)
{
    uint8_t data[6] = {0};
    uint32_t hum_raw = 0, temp_raw = 0;
    
    // 等待测量完成
    for(int i = 0; i < 20; i++)
    {
        HAL_Delay(5);
        if((AHT20_Read_Status() & 0x80) == 0)
        {
            break;
        }
        if(i == 19) return 2;
    }
    
    // 读取6字节数据
    if(HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS_READ, data, 6, 100) != HAL_OK)
    {
        return 1;
    }
    
    // 解析湿度数据
    hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4);
    *humidity = (float)hum_raw * 100.0f / 1048576.0f;
    
    // 解析温度数据
    temp_raw = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
    *temperature = (float)temp_raw * 200.0f / 1048576.0f - 50.0f;
    
    // 数据范围检查
    if(*humidity < 0.0f || *humidity > 100.0f || *temperature < -40.0f || *temperature > 85.0f)
    {
        return 1;
    }
    
    return 0;
}

// 完整的温湿度读取流程
void AHT20_Read_Process(float *temp, float *humi)
{
    static uint8_t init_flag = 0;
    
    if(init_flag == 0)
    {
        if(AHT20_Init() == 0)
        {
            init_flag = 1;
        }
    }
    
    if(init_flag)
    {
        if(AHT20_Start_Measurement() == 0)
        {
            HAL_Delay(80);
            AHT20_Read_Data(temp, humi);
        }
    }
}