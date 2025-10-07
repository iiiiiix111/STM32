#include "stm32f10x.h"
#include "board.h"
#include "delay.h"

int main(void)
{
    board_init();

    // 使能GPIOA和GPIOC的时钟
    RCC->APB2ENR |= (1 << 2) | (1 << 4);

    // 配置PA0、PA1、PA2为推挽输出（低位引脚在CRL中）
    // 清除对应位
    GPIOA->CRL &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)) | (0xF << (2 * 4)));
    // 设定模式：输出模式 10MHz 推挽输出（0x1）——注意：可根据需要调整速度
    GPIOA->CRL |= ((0x1 << (0 * 4)) | (0x1 << (1 * 4)) | (0x1 << (2 * 4)));

    // 配置PC13为推挽输出（高位引脚，在CRH中）
    GPIOC->CRH &= ~(0xF << ((13 - 8) * 4));
    GPIOC->CRH |=  (0x1 << ((13 - 8) * 4));

    // 初始状态：全部置为1（LED灭）
    GPIOA->ODR |= ((1 << 0) | (1 << 1) | (1 << 2));
    GPIOC->ODR |= (1 << 13);

    while(1)
    {
        // Step 1: 点亮PC13（低电平有效），其他保持灭（高电平）
        GPIOC->ODR &= ~(1 << 13);               // PC13低，点亮LED
        GPIOA->ODR |= ((1 << 0) | (1 << 1) | (1 << 2)); // PA0, PA1, PA2置高，LED熄灭
        delay_ms(1000);

        // Step 2: 点亮PA0，其他保持灭
        GPIOC->ODR |= (1 << 13);                // PC13高，熄灭LED
        GPIOA->ODR &= ~(1 << 0);                // PA0低，点亮LED
        GPIOA->ODR |= ((1 << 1) | (1 << 2));      // PA1, PA2高，LED熄灭
        delay_ms(1000);

        // Step 3: 点亮PA1，其他保持灭
        GPIOA->ODR |= (1 << 0);                 // PA0高，熄灭LED
        GPIOA->ODR &= ~(1 << 1);                // PA1低，点亮LED
        GPIOA->ODR |= (1 << 2);                 // PA2高，熄灭LED
        // PC13已经保持高
        delay_ms(1000);

        // Step 4: 点亮PA2，其他保持灭
        GPIOC->ODR |= (1 << 13);                // PC13高，熄灭LED
        GPIOA->ODR |= ((1 << 0) | (1 << 1));      // PA0, PA1高，熄灭LED
        GPIOA->ODR &= ~(1 << 2);                // PA2低，点亮LED
        delay_ms(1000);
    }
}