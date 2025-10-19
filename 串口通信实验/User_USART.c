
#include "User_USART.h"
#include "stm32f10x.h"


//USART使用到的GPIO口设置
void User_USART_GPIO_Config(void)
{
    GPIO_InitTypeDef USART_GPIO_TX,USART_GPIO_RX;

    //先使能USART对应的GPIO口使用，USART使用的都是GPIOA组
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    //初始化GPIO_InitTypeDef结构体--USART TX PIN
    USART_GPIO_TX.GPIO_Mode = GPIO_Mode_AF_PP;
    USART_GPIO_TX.GPIO_Pin = GPIO_Pin_9;
    USART_GPIO_TX.GPIO_Speed = GPIO_Speed_50MHz;
    
    //设置USART TX PIN对应的控制寄存器
    GPIO_Init(GPIOA, &USART_GPIO_TX);

    
    //初始化GPIO_InitTypeDef结构体--USART RX PIN  此为输入，所以不用初始化速率GPIO_Speed成员
    USART_GPIO_RX.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    USART_GPIO_RX.GPIO_Pin = GPIO_Pin_10;
    
    //设置USART RX PIN对应的控制寄存器
    GPIO_Init(GPIOA, &USART_GPIO_RX);


}


//USART相关寄存器设置
void User_USART_Config(void)
{

    USART_InitTypeDef USART_Config;

    //使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //初始化USART_InitTypeDef结构体
    USART_Config.USART_BaudRate = 115200;
    USART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Config.USART_Parity = USART_Parity_No;
    USART_Config.USART_StopBits = USART_StopBits_1;
    USART_Config.USART_WordLength = USART_WordLength_8b;

    //设置USART1对应的寄存器
    USART_Init(USART1, &USART_Config);

    //使能USART1 数据接收RXEN中断寄存器，即接收数据寄存器有数据可读时，RXEN寄存器产生中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    //使能串口USART1，使其进行工作
    USART_Cmd(USART1, ENABLE);
    
    
}


//向串口发送一个字节数据
void User_USART_Send_Byte(USART_TypeDef* pUSARTX, uint8_t Data)
{

    //向数据寄存器写入8bit数据
    pUSARTX->DR = (Data & (uint16_t)0x01FF);    

    //USART_GetFlagStatus检查数据是否发送完成
    while(USART_GetFlagStatus(pUSARTX, USART_FLAG_TXE) == RESET);
    
}


//向串口发送一个字符串数据，即可以发送包含多个字节的数据，char类型为8bit，其字符串中的每个字符都可用一个int数表示，即ASCII标准
void User_UART_Send_String(USART_TypeDef* pUSARTX, char* str)
{
    int i;

    for(i = 0; i < sizeof(str); i++)
    {
        User_USART_Send_Byte(pUSARTX, *(str + i));

        //USART_GetFlagStatus检查多个数据是否发送完成
        while(USART_GetFlagStatus(pUSARTX, USART_FLAG_TC) == RESET);


    }

}