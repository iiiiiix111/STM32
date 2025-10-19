#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x.h"

// 全局变量
int g_init = 1;
int g_uninit;
const int g_const = 100;

void func() {
    int local = 10;               // 栈变量
    static int static_var = 20;   // 静态变量
    int *heap = malloc(4);        // 堆变量

    printf("local: %p\r\n", &local);
    printf("static_var: %p\r\n", &static_var);
    printf("heap: %p\r\n", heap);

    free(heap);
}

int main() {
    // 初始化串口
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpio);

    USART_InitTypeDef usart;
    usart.USART_BaudRate = 115200;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);

    printf("g_init: %p\r\n", &g_init);
    printf("g_uninit: %p\r\n", &g_uninit);
    printf("g_const: %p\r\n", &g_const);

    func();

    while (1);
}
