#include "stm32f10x.h"
#include "Delay.h"

int main(void)
{
    // 1. 打开 GPIOA 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 初始化 PA0、PA1、PA2 为推挽输出
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 流水灯循环
    while(1)
    {
        GPIO_Write(GPIOA, ~(0x0001));   // 点亮 PA0（蓝灯）
			
        Delay_ms(1000);

        GPIO_Write(GPIOA, ~(0x0002));   // 点亮 PA1（绿灯）
        Delay_ms(1000);

        GPIO_Write(GPIOA, ~(0x0004));   // 点亮 PA2（红灯）
        Delay_ms(1000);
    }
}
