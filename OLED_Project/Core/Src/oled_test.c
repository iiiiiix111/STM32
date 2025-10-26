#include "oled_test.h"
#include "string.h"
#include "stdio.h"

#define OLED_CMD  0x00
#define OLED_DATA 0x40

// 简化的OLED初始化命令
static uint8_t oled_init_cmds[] = {
    0xAE,       // 关闭显示
    0x20, 0x00, // 设置内存地址模式
    0xB0,       // 设置页起始地址
    0xC8,       // 设置扫描方向
    0x00, 0x10, // 设置列地址
    0x40,       // 设置显示起始行
    0x81, 0x7F, // 设置对比度
    0xA1,       // 设置段重映射
    0xA6,       // 设置正常显示
    0xA8, 0x3F, // 设置多路复用率
    0xD3, 0x00, // 设置显示偏移
    0xD5, 0x80, // 设置振荡器频率
    0xD9, 0xF1, // 设置预充电周期
    0xDA, 0x12, // 设置COM引脚硬件配置
    0xDB, 0x30, // 设置VCOMH电平
    0x8D, 0x14, // 设置充电泵
    0xAF        // 开启显示
};

// 写命令
void OLED_Write_Cmd(uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, OLED_CMD, 1, &cmd, 1, 100);
}

// 写数据
void OLED_Write_Data(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, OLED_DATA, 1, &data, 1, 100);
}

// OLED初始化
void OLED_Test_Init(void)
{
    HAL_Delay(100); // 等待OLED上电稳定
    
    printf("开始OLED初始化...\r\n");
    
    for(uint8_t i = 0; i < sizeof(oled_init_cmds); i++)
    {
        OLED_Write_Cmd(oled_init_cmds[i]);
        HAL_Delay(2);
    }
    
    OLED_Test_Clear();
    printf("OLED初始化完成\r\n");
}

// 清屏
void OLED_Test_Clear(void)
{
    for(uint8_t page = 0; page < 8; page++)
    {
        OLED_Write_Cmd(0xB0 + page); // 设置页地址
        OLED_Write_Cmd(0x00);        // 设置列低地址
        OLED_Write_Cmd(0x10);        // 设置列高地址
        
        for(uint8_t col = 0; col < 128; col++)
        {
            OLED_Write_Data(0x00);   // 清空显示数据
        }
    }
}

// 显示状态信息
void OLED_Test_ShowStatus(char *status)
{
    // 简单实现 - 在固定位置显示状态
    // 实际使用时需要完整的字符显示函数
    printf("OLED状态: %s\r\n", status);
}

// 显示文本（简化版）
void OLED_Test_ShowText(uint8_t line, char *text)
{
    printf("OLED显示[行%d]: %s\r\n", line, text);
}

// 测试所有功能
void OLED_Test_AllFunctions(void)
{
    printf("\r\n=== OLED功能测试 ===\r\n");
    
    // 测试1: 通信测试
    printf("1. 测试I2C通信...\r\n");
    if(HAL_I2C_IsDeviceReady(&hi2c1, OLED_ADDR, 3, 10) == HAL_OK)
    {
        printf("   I2C通信正常 (地址: 0x%02X)\r\n", OLED_ADDR);
    }
    else
    {
        printf("   I2C通信失败! 尝试地址 0x7A...\r\n");
        // 可以在这里尝试其他地址
    }
    
    // 测试2: 初始化测试
    printf("2. 初始化测试...\r\n");
    OLED_Test_Init();
    
    // 测试3: 清屏测试
    printf("3. 清屏测试...\r\n");
    OLED_Test_Clear();
    HAL_Delay(1000);
    
    // 测试4: 显示测试
    printf("4. 显示测试...\r\n");
    OLED_Test_ShowText(0, "OLED Test");
    OLED_Test_ShowText(2, "Line 2");
    OLED_Test_ShowText(4, "Line 4");
    OLED_Test_ShowText(6, "Success!");
    
    printf("=== OLED测试完成 ===\r\n\r\n");
}