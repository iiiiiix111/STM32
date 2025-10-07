#include "stm32f10x.h"
#include "board.h"
#include "stdio.h"

int main(void)
{
    board_init(); 

    // ʹ��GPIOʱ��
    RCC->APB2ENR |= (1 << 2)  // GPIOA
                 | (1 << 3)   // GPIOB  
                 | (1 << 4);  // GPIOC
	
    // ����PC13Ϊ����������޸�ΪPC13��
    GPIOC->CRH &= ~(0xF << (4 * (13 - 8))); 
    GPIOC->CRH |=  (0x1 << (4 * (13 - 8)));  

    // ����PA0Ϊ�����������8λ��ʹ��CRL�Ĵ�����
    GPIOA->CRL &= ~(0xF << (4 * 0)); 
    GPIOA->CRL |=  (0x1 << (4 * 0)); 

    // ����PB0Ϊ�����������8λ��ʹ��CRL�Ĵ�����
    GPIOB->CRL &= ~(0xF << (4 * 0)); 
    GPIOB->CRL |=  (0x1 << (4 * 0)); 

    // ��ʼ�����״̬
    GPIOC->ODR |=  (1 << 13);  // PC13�ߵ�ƽ
    GPIOA->ODR |=  (1 << 0);   // PA0�ߵ�ƽ  
    GPIOB->ODR |=  (1 << 0);   // PB0�ߵ�ƽ
	
    while (1)
    {
        // ״̬1��PC13�ͣ�PA0�ߣ�PB0�� ���̵���˸��
        GPIOC->ODR &= ~(1 << 13); 
        GPIOA->ODR |=  (1 << 0);  
        GPIOB->ODR |=  (1 << 0); 
        delay_ms(1000);

        // ״̬2��PC13�ߣ�PA0�ͣ�PB0�� ��������˸��
        GPIOC->ODR |=  (1 << 13); 
        GPIOA->ODR &= ~(1 << 0);   
        GPIOB->ODR |=  (1 << 0);  
        delay_ms(1000);

        // ״̬3��PC13�ߣ�PA0�ߣ�PB0�� �������˸��
        GPIOC->ODR |=  (1 << 13); 
        GPIOA->ODR |=  (1 << 0);  
        GPIOB->ODR &= ~(1 << 0); 
        delay_ms(1000);
    }
}