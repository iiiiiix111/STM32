#include "oled.h"
#include "string.h"
#include "stdio.h"

#define OLED_CMD  0x00
#define OLED_DATA 0x40

// SSD1306初始化命令
static uint8_t oled_init_cmds[] = {
    0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 
    0xA6, 0xA8, 0x3F, 0xC8, 0xD3, 0x00, 0xD5, 0x80, 
    0xD9, 0xF1, 0xDA, 0x12, 0xDB, 0x40, 0x8D, 0x14, 
    0xAF
};

// 你的12x12字模数据（列扫描）
// 汉字 (12x12 - 22字节)
static const uint8_t HZ_Wen[] = {  // "温" - 0
  0x22,0x04,0x44,0x02,0x00,0x01,0xC0,0x07,
  0x5F,0x00,0xD5,0x07,0x55,0x00,0xD5,0x07,
  0x5F,0x00,0xC0,0x07,0x00,0x00
};

static const uint8_t HZ_Shi[] = {  // "湿" - 1
  0x11,0x04,0x22,0x02,0x80,0x00,0x3E,0x01,
  0x2A,0x00,0xEA,0x07,0x2A,0x00,0xEA,0x07,
  0x2A,0x00,0x3E,0x01,0x80,0x00
};

static const uint8_t HZ_Du[] = {   // "度" - 2
  0x00,0x00,0xFE,0x07,0x0A,0x00,0x8A,0x00,
  0xBE,0x01,0xAA,0x02,0xAB,0x04,0xAA,0x04,
  0xBE,0x02,0x8A,0x01,0x0A,0x00
};

// 数字 0-9 (10字节)
static const uint8_t font_0[] = {0xFC,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0xFC,0x01};
static const uint8_t font_1[] = {0x00,0x00,0x04,0x02,0xFE,0x03,0x00,0x02,0x00,0x00};
static const uint8_t font_2[] = {0x0C,0x03,0x82,0x02,0x42,0x02,0x22,0x02,0x1C,0x03};
static const uint8_t font_3[] = {0x04,0x01,0x02,0x02,0x22,0x02,0x22,0x02,0xDC,0x01};
static const uint8_t font_4[] = {0xC0,0x00,0xB0,0x00,0x88,0x02,0xFE,0x03,0x80,0x02};
static const uint8_t font_5[] = {0xBE,0x01,0x12,0x02,0x12,0x02,0x12,0x02,0xE2,0x01};
static const uint8_t font_6[] = {0xF8,0x01,0x24,0x02,0x12,0x02,0x12,0x02,0xE4,0x01};
static const uint8_t font_7[] = {0x06,0x00,0x02,0x00,0xC2,0x03,0x3A,0x00,0x06,0x00};
static const uint8_t font_8[] = {0xDC,0x01,0x22,0x02,0x22,0x02,0x22,0x02,0xDC,0x01};
static const uint8_t font_9[] = {0x3C,0x01,0x42,0x02,0x42,0x02,0x22,0x01,0xFC,0x00};

// 符号
static const uint8_t font_dot[] = {0x00,0x00,0x00,0x00,0x60,0x00,0x60,0x00,0x00,0x00};
static const uint8_t font_C[] = {  // "℃"
  0x04,0x00,0x0A,0x00,0x04,0x00,0xF0,0x01,
  0x08,0x02,0x04,0x04,0x04,0x04,0x04,0x04,
  0x08,0x04,0x1C,0x02,0x00,0x01
};
static const uint8_t font_percent[] = {  // "％"
  0x00,0x00,0x1C,0x02,0x22,0x01,0xA2,0x00,
  0x5C,0x00,0xA0,0x03,0x50,0x04,0x48,0x04,
  0x84,0x03,0x02,0x00,0x00,0x00
};
static const uint8_t font_space[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t font_colon[] = {0x00,0x00,0x00,0x00,0x66,0x00,0x66,0x00,0x00,0x00}; // 冒号

void OLED_Write_Cmd(uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, OLED_CMD, 1, &cmd, 1, 100);
}

void OLED_Write_Data(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, OLED_DATA, 1, &data, 1, 100);
}

void OLED_Init(void)
{
    HAL_Delay(100);
    for(int i = 0; i < sizeof(oled_init_cmds); i++)
    {
        OLED_Write_Cmd(oled_init_cmds[i]);
        HAL_Delay(1);
    }
    OLED_Clear();
}

void OLED_Clear(void)
{
    for(int page = 0; page < 8; page++)
    {
        OLED_Write_Cmd(0xB0 + page);
        OLED_Write_Cmd(0x00);
        OLED_Write_Cmd(0x10);
        for(int col = 0; col < 128; col++)
        {
            OLED_Write_Data(0x00);
        }
    }
}

// 修正版：显示12x12字符（列扫描数据）
void OLED_ShowChar12x12(uint8_t x, uint8_t y, const uint8_t *font_data, uint8_t data_len)
{
    // 对于12x12字符，每列2字节（16位），但只使用前12位
    // 数据按列排列：第0列(字节0-1), 第1列(字节2-3), ..., 第11列(字节22-23)
    
    for(uint8_t col = 0; col < 12; col++)
    {
        // 计算当前列的字节偏移
        uint8_t byte_offset = col * 2;
        if(byte_offset + 1 >= data_len) break;
        
        // 获取当前列的两个字节数据
        uint8_t upper_byte = font_data[byte_offset];
        uint8_t lower_byte = font_data[byte_offset + 1];
        
        // 设置列地址
        OLED_Write_Cmd(0xB0 + y);          // 页地址（上半部分）
        OLED_Write_Cmd((x + col) & 0x0F);  // 列低地址
        OLED_Write_Cmd(0x10 | ((x + col) >> 4)); // 列高地址
        
        // 发送上半部分数据（前8位）
        OLED_Write_Data(upper_byte);
        
        // 设置到下一页（下半部分）
        OLED_Write_Cmd(0xB0 + y + 1);      // 页地址（下半部分）
        OLED_Write_Cmd((x + col) & 0x0F);  // 列低地址
        OLED_Write_Cmd(0x10 | ((x + col) >> 4)); // 列高地址
        
        // 发送下半部分数据（后4位，只取低4位）
        OLED_Write_Data(lower_byte & 0x0F);
    }
}

// 根据数字获取对应的字模
const uint8_t* OLED_GetDigitFont(uint8_t digit)
{
    switch(digit)
    {
        case 0: return font_0;
        case 1: return font_1;
        case 2: return font_2;
        case 3: return font_3;
        case 4: return font_4;
        case 5: return font_5;
        case 6: return font_6;
        case 7: return font_7;
        case 8: return font_8;
        case 9: return font_9;
        default: return font_0;
    }
}

// 显示温度（第一行）
void OLED_ShowTemperature(float temp)
{
    // 计算居中位置："温度：XX℃" 总共6个字符宽度 (12*6=72像素)
    uint8_t start_x = (128 - 72) / 2;
    uint8_t y = 0; // 第一行（页0-1）
    
    // 显示"温度："
    OLED_ShowChar12x12(start_x, y, HZ_Wen, 22);      // 温
    OLED_ShowChar12x12(start_x + 12, y, HZ_Du, 22);  // 度
    OLED_ShowChar12x12(start_x + 24, y, font_colon, 10); // 冒号
    
    // 显示温度数值
    int temp_int = (int)temp;
    
    // 十位数（如果有）
    if(temp_int >= 10)
    {
        OLED_ShowChar12x12(start_x + 36, y, OLED_GetDigitFont(temp_int / 10), 10);
        OLED_ShowChar12x12(start_x + 48, y, OLED_GetDigitFont(temp_int % 10), 10);
    }
    else
    {
        // 只有个位数，前面补空格保持居中
        OLED_ShowChar12x12(start_x + 36, y, font_space, 10);
        OLED_ShowChar12x12(start_x + 48, y, OLED_GetDigitFont(temp_int), 10);
    }
    
    // 显示℃符号
    OLED_ShowChar12x12(start_x + 60, y, font_C, 22);
}

// 显示湿度（第二行）
void OLED_ShowHumidity(float humi)
{
    // 计算居中位置："湿度：XX％" 总共6个字符宽度 (12*6=72像素)
    uint8_t start_x = (128 - 72) / 2;
    uint8_t y = 2; // 第二行（页2-3）
    
    // 显示"湿度："
    OLED_ShowChar12x12(start_x, y, HZ_Shi, 22);      // 湿
    OLED_ShowChar12x12(start_x + 12, y, HZ_Du, 22);  // 度
    OLED_ShowChar12x12(start_x + 24, y, font_colon, 10); // 冒号
    
    // 显示湿度数值
    int humi_int = (int)humi;
    
    // 十位数（如果有）
    if(humi_int >= 10)
    {
        OLED_ShowChar12x12(start_x + 36, y, OLED_GetDigitFont(humi_int / 10), 10);
        OLED_ShowChar12x12(start_x + 48, y, OLED_GetDigitFont(humi_int % 10), 10);
    }
    else
    {
        // 只有个位数，前面补空格保持居中
        OLED_ShowChar12x12(start_x + 36, y, font_space, 10);
        OLED_ShowChar12x12(start_x + 48, y, OLED_GetDigitFont(humi_int), 10);
    }
    
    // 显示%符号
    OLED_ShowChar12x12(start_x + 60, y, font_percent, 22);
}

// 显示温湿度信息（居中显示）
void OLED_ShowTempHumi(float temp, float humi)
{
    OLED_Clear();
    
    // 第一行：温度
    OLED_ShowTemperature(temp);
    
    // 第二行：湿度
    OLED_ShowHumidity(humi);
}

// 测试函数：显示所有字符用于调试
void OLED_TestDisplay(void)
{
    OLED_Clear();
    
    uint8_t x = 0;
    uint8_t y = 0;
    
    // 显示汉字
    OLED_ShowChar12x12(x, y, HZ_Wen, 22);
    OLED_ShowChar12x12(x + 12, y, HZ_Shi, 22);
    OLED_ShowChar12x12(x + 24, y, HZ_Du, 22);
    
    // 显示数字
    y = 2;
    for(int i = 0; i < 5; i++)
    {
        OLED_ShowChar12x12(x + i * 12, y, OLED_GetDigitFont(i), 10);
    }
    for(int i = 5; i < 10; i++)
    {
        OLED_ShowChar12x12(x + (i-5) * 12, y + 2, OLED_GetDigitFont(i), 10);
    }
    
    // 显示符号
    y = 4;
    OLED_ShowChar12x12(x, y, font_C, 22);
    OLED_ShowChar12x12(x + 12, y, font_percent, 22);
    OLED_ShowChar12x12(x + 24, y, font_dot, 10);
    OLED_ShowChar12x12(x + 36, y, font_colon, 10);
}