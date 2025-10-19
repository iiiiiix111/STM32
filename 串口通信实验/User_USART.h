#ifndef __USER_USART_H
  #define __USER_USART_H
  #include "stm32f10x.h"

 void User_USART_GPIO_Config(void); //USART使用到的GPIO口设置
 void User_USART_Config(void);    //USART相关寄存器设置
 void User_USART_Send_Byte(USART_TypeDef* pUSARTX, uint8_t Data);     //向串口发送一个字节数据
 void User_UART_Send_String(USART_TypeDef* pUSARTX, char* str);        //向串口发送一个字符串数据，即可以发送包含多个字节的数据	
 #endif