#include "NVIC.h"
#include "User_USART.h"
#include "stm32f10x.h"

int main(void)
{

    User_USART_GPIO_Config();
    User_NVIC_Config();
    User_USART_Config();

	
    User_UART_Send_String(USART1, "hello windows!\n");

    while(1)
    {
    }

}