  #include "NVIC.h"
  #include "stm32f10x.h"
  
  void User_NVIC_Config(void)
  {
      NVIC_InitTypeDef NVIC_Config;
      
      //设置中断控制器优先级组类别
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
      //初始化NVIC_InitTypeDef结构体
     NVIC_Config.NVIC_IRQChannel = USART1_IRQn;
     NVIC_Config.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_Config.NVIC_IRQChannelSubPriority = 1;
     NVIC_Config.NVIC_IRQChannelCmd = ENABLE;
 
     //初始化NVIC中断控制器
     NVIC_Init(&NVIC_Config);
     
 }  