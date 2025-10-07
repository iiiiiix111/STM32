#include "stm32f10x.h"
#include "board.h"
#include "delay.h"

int main(void)
{
    board_init();

    // ʹ��GPIOA��GPIOC��ʱ��
    RCC->APB2ENR |= (1 << 2) | (1 << 4);

    // ����PA0��PA1��PA2Ϊ�����������λ������CRL�У�
    // �����Ӧλ
    GPIOA->CRL &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)) | (0xF << (2 * 4)));
    // �趨ģʽ�����ģʽ 10MHz ���������0x1������ע�⣺�ɸ�����Ҫ�����ٶ�
    GPIOA->CRL |= ((0x1 << (0 * 4)) | (0x1 << (1 * 4)) | (0x1 << (2 * 4)));

    // ����PC13Ϊ�����������λ���ţ���CRH�У�
    GPIOC->CRH &= ~(0xF << ((13 - 8) * 4));
    GPIOC->CRH |=  (0x1 << ((13 - 8) * 4));

    // ��ʼ״̬��ȫ����Ϊ1��LED��
    GPIOA->ODR |= ((1 << 0) | (1 << 1) | (1 << 2));
    GPIOC->ODR |= (1 << 13);

    while(1)
    {
        // Step 1: ����PC13���͵�ƽ��Ч�������������𣨸ߵ�ƽ��
        GPIOC->ODR &= ~(1 << 13);               // PC13�ͣ�����LED
        GPIOA->ODR |= ((1 << 0) | (1 << 1) | (1 << 2)); // PA0, PA1, PA2�øߣ�LEDϨ��
        delay_ms(1000);

        // Step 2: ����PA0������������
        GPIOC->ODR |= (1 << 13);                // PC13�ߣ�Ϩ��LED
        GPIOA->ODR &= ~(1 << 0);                // PA0�ͣ�����LED
        GPIOA->ODR |= ((1 << 1) | (1 << 2));      // PA1, PA2�ߣ�LEDϨ��
        delay_ms(1000);

        // Step 3: ����PA1������������
        GPIOA->ODR |= (1 << 0);                 // PA0�ߣ�Ϩ��LED
        GPIOA->ODR &= ~(1 << 1);                // PA1�ͣ�����LED
        GPIOA->ODR |= (1 << 2);                 // PA2�ߣ�Ϩ��LED
        // PC13�Ѿ����ָ�
        delay_ms(1000);

        // Step 4: ����PA2������������
        GPIOC->ODR |= (1 << 13);                // PC13�ߣ�Ϩ��LED
        GPIOA->ODR |= ((1 << 0) | (1 << 1));      // PA0, PA1�ߣ�Ϩ��LED
        GPIOA->ODR &= ~(1 << 2);                // PA2�ͣ�����LED
        delay_ms(1000);
    }
}