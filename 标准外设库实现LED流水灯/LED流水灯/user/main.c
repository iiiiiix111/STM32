#include "stm32f10x.h"
#include "Delay.h"

int main(void)
{
    // 1. �� GPIOA ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. ��ʼ�� PA0��PA1��PA2 Ϊ�������
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. ��ˮ��ѭ��
    while(1)
    {
        GPIO_Write(GPIOA, ~(0x0001));   // ���� PA0�����ƣ�
			
        Delay_ms(1000);

        GPIO_Write(GPIOA, ~(0x0002));   // ���� PA1���̵ƣ�
        Delay_ms(1000);

        GPIO_Write(GPIOA, ~(0x0004));   // ���� PA2����ƣ�
        Delay_ms(1000);
    }
}
