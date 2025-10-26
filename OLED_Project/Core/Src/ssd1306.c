#include "ssd1306.h"
#include "string.h"
#include "stdio.h"

#define SSD1306_CMD     0x00
#define SSD1306_DATA    0x40

// SSD1306初始化命令序列
static const uint8_t ssd1306_init_cmds[] = {
    0xAE,           // 关闭显示
    0x20, 0x00,     // 设置内存地址模式: 水平地址模式
    0xB0,           // 设置页起始地址
    0xC8,           // 设置COM扫描方向
    0x00,           // 设置列低地址
    0x10,           // 设置列高地址  
    0x40,           // 设置显示起始行
    0x81, 0x7F,     // 设置对比度
    0xA1,           // 设置段重映射
    0xA6,           // 设置正常显示
    0xA8, 0x3F,     // 设置多路复用率
    0xD3, 0x00,     // 设置显示偏移
    0xD5, 0x80,     // 设置显示时钟分频
    0xD9, 0xF1,     // 设置预充电周期
    0xDA, 0x12,     // 设置COM引脚硬件配置
    0xDB, 0x30,     // 设置VCOMH电平
    0x8D, 0x14,     // 设置充电泵
    0xAF            // 开启显示
};

// 写命令
static void SSD1306_Write_Cmd(uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, SSD1306_CMD, 1, &cmd, 1, 100);
}

// 写数据
static void SSD1306_Write_Data(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, SSD1306_DATA, 1, &data, 1, 100);
}

// SSD1306初始化
uint8_t SSD1306_Init(void)
{
    // 测试I2C通信
    if(HAL_I2C_IsDeviceReady(&hi2c1, SSD1306_I2C_ADDR, 3, 10) != HAL_OK)
    {
        return 1; // 通信失败
    }
    
    HAL_Delay(100); // 等待OLED上电稳定
    
    // 发送初始化命令
    for(uint16_t i = 0; i < sizeof(ssd1306_init_cmds); i++)
    {
        SSD1306_Write_Cmd(ssd1306_init_cmds[i]);
        HAL_Delay(1);
    }
    
    SSD1306_Clear();
    return 0; // 成功
}

// 清屏
void SSD1306_Clear(void)
{
    for(uint8_t page = 0; page < 8; page++)
    {
        SSD1306_Write_Cmd(0xB0 + page); // 设置页地址
        SSD1306_Write_Cmd(0x00);        // 设置列低地址
        SSD1306_Write_Cmd(0x10);        // 设置列高地址
        
        // 清空整个页（128列）
        for(uint8_t col = 0; col < 128; col++)
        {
            SSD1306_Write_Data(0x00);
        }
    }
}

// 测试图案 - 显示网格图案验证OLED工作
void SSD1306_Test_Pattern(void)
{
    for(uint8_t page = 0; page < 8; page++)
    {
        SSD1306_Write_Cmd(0xB0 + page);
        SSD1306_Write_Cmd(0x00);
        SSD1306_Write_Cmd(0x10);
        
        for(uint8_t col = 0; col < 128; col++)
        {
            // 创建测试图案：每隔8个像素点亮一个点
            uint8_t pattern = (col % 16 == 0) ? 0xFF : 0x00;
            SSD1306_Write_Data(pattern);
        }
    }
}

// 显示简单消息（基础版本）
void SSD1306_Show_Message(char* line1, char* line2, char* line3, char* line4)
{
    // 这里实现简单的文本显示
    // 暂时用测试图案代替
    SSD1306_Test_Pattern();
}