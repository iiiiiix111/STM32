#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"   // ͷ�ļ�


#define __STM32F10x_CONF_H
#include ".\RTE\Device\STM32F103C8\stm32f10x_conf.h"

void GPIO_Config(void);
void delay_ms(int ms);

int main(void)
{
    // ��ʼ�� GPIO
    GPIO_Config();

    while (1)
    {
        // LED1
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        delay_ms(300);
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);

        // LED2
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        delay_ms(300);
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);

        // LED3
        GPIO_SetBits(GPIOA, GPIO_Pin_2);
        delay_ms(300);
        GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    }
}

void GPIO_Config(void)
{
    // 1. ���� GPIOA ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. ���� PA0, PA1, PA2 Ϊ�������
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   // �������
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // ����ٶ�
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void delay_ms(int ms)
{
    int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 7200; j++) // ������ʱ��72MHz ʱ��
        {
            __NOP();
        }
    }
}
