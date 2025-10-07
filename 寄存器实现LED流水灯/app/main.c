#include "stm32f10x.h"
#include "board.h"
#include "stdio.h"

int main(void)
{
    board_init(); 

    // 使能GPIO时钟
    RCC->APB2ENR |= (1 << 2)  // GPIOA
                 | (1 << 3)   // GPIOB  
                 | (1 << 4);  // GPIOC
	
    // 配置PC13为推挽输出（修改为PC13）
    GPIOC->CRH &= ~(0xF << (4 * (13 - 8))); 
    GPIOC->CRH |=  (0x1 << (4 * (13 - 8)));  

    // 配置PA0为推挽输出（低8位，使用CRL寄存器）
    GPIOA->CRL &= ~(0xF << (4 * 0)); 
    GPIOA->CRL |=  (0x1 << (4 * 0)); 

    // 配置PB0为推挽输出（低8位，使用CRL寄存器）
    GPIOB->CRL &= ~(0xF << (4 * 0)); 
    GPIOB->CRL |=  (0x1 << (4 * 0)); 

    // 初始化输出状态
    GPIOC->ODR |=  (1 << 13);  // PC13高电平
    GPIOA->ODR |=  (1 << 0);   // PA0高电平  
    GPIOB->ODR |=  (1 << 0);   // PB0高电平
	
    while (1)
    {
        // 状态1：PC13低，PA0高，PB0高 （绿灯闪烁）
        GPIOC->ODR &= ~(1 << 13); 
        GPIOA->ODR |=  (1 << 0);  
        GPIOB->ODR |=  (1 << 0); 
        delay_ms(1000);

        // 状态2：PC13高，PA0低，PB0高 （蓝灯闪烁）
        GPIOC->ODR |=  (1 << 13); 
        GPIOA->ODR &= ~(1 << 0);   
        GPIOB->ODR |=  (1 << 0);  
        delay_ms(1000);

        // 状态3：PC13高，PA0高，PB0低 （红灯闪烁）
        GPIOC->ODR |=  (1 << 13); 
        GPIOA->ODR |=  (1 << 0);  
        GPIOB->ODR &= ~(1 << 0); 
        delay_ms(1000);
    }
}