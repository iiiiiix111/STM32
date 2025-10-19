/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DMA_BUFFER_SIZE 128
#define STATUS_UPDATE_INTERVAL 2000  // 状态更新间隔(ms)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// DMA传输变量
uint8_t dma_buffer[DMA_BUFFER_SIZE];  // DMA循环发送缓冲区
volatile uint32_t dma_half_transfer_count = 0;
volatile uint32_t dma_full_transfer_count = 0;
volatile uint8_t dma_active = 0;
uint32_t last_status_time = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void DMA_CheckConfig(void);
void Buffer_Init(void);
void Send_Status_Info(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();      // 先初始化DMA
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // 保护SWD调试接口
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  // 初始化DMA缓冲区
  Buffer_Init();
  
  // 发送启动信息
  HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
  HAL_UART_Transmit(&huart1, (uint8_t*)"========================================\r\n", 42, 100);
  HAL_UART_Transmit(&huart1, (uint8_t*)"STM32 DMA Continuous Transmission Test\r\n", 40, 100);
  HAL_UART_Transmit(&huart1, (uint8_t*)"Baud Rate: 115200 bps | Mode: Circular\r\n", 40, 100);
  HAL_UART_Transmit(&huart1, (uint8_t*)"========================================\r\n", 42, 100);
  
  HAL_Delay(100);
  
  // 检查DMA配置
  DMA_CheckConfig();
  
  // 启动DMA循环传输
  HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&huart1, dma_buffer, DMA_BUFFER_SIZE);
  
  if (status == HAL_OK) {
      HAL_UART_Transmit(&huart1, (uint8_t*)"DMA Circular transmission STARTED\r\n", 35, 100);
      HAL_UART_Transmit(&huart1, (uint8_t*)"Data streaming continuously...\r\n\r\n", 33, 100);
      dma_active = 1;
  } else {
      HAL_UART_Transmit(&huart1, (uint8_t*)"DMA start FAILED! Error: ", 25, 100);
      char error_msg[10];
      sprintf(error_msg, "%d\r\n", status);
      HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 100);
      Error_Handler();
  }
  
  last_status_time = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 主循环 - DMA在后台自动循环发送数据
    
    // 定期发送状态信息（不干扰DMA传输）
    uint32_t current_time = HAL_GetTick();
    if (current_time - last_status_time >= STATUS_UPDATE_INTERVAL) {
        if (dma_active) {
            Send_Status_Info();
        }
        last_status_time = current_time;
    }
    
    // LED闪烁指示系统运行（如果开发板有LED）
    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    // HAL_Delay(500);
    
    // 可以在这里添加其他任务
    HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief 初始化DMA缓冲区
  */
/**
  * @brief 初始化DMA缓冲区 - 紧凑版本
  */
void Buffer_Init(void)
{
    // 清空缓冲区
    memset(dma_buffer, 0, DMA_BUFFER_SIZE);
    
    // 创建多行内容
    snprintf((char*)dma_buffer, DMA_BUFFER_SIZE, 
        "STM32 DMA Test - Circular Mode\r\n"
        "Data1:ABCD  Data2:EFGH  Data3:IJKL\r\n" 
        "Data4:MNOP  Count:000000  Status:OK\r\n"
        "Baud:115200  Time:%08lu\r\n"
        "--------------------------------\r\n",
        HAL_GetTick());
    
    // 确保以\r\n结束
    int len = strlen((char*)dma_buffer);
    if(len < DMA_BUFFER_SIZE - 2) {
        dma_buffer[len] = '\r';
        dma_buffer[len+1] = '\n';
    } else {
        dma_buffer[DMA_BUFFER_SIZE-2] = '\r';
        dma_buffer[DMA_BUFFER_SIZE-1] = '\n';
    }
}

/**
  * @brief 检查DMA配置
  */
void DMA_CheckConfig(void)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)"Checking DMA configuration...\r\n", 31, 100);
    
    if(huart1.hdmatx->Init.Mode == DMA_CIRCULAR) {
        HAL_UART_Transmit(&huart1, (uint8_t*)"? DMA Mode: Circular\r\n", 23, 100);
    } else {
        HAL_UART_Transmit(&huart1, (uint8_t*)"? DMA Mode: Normal (Should be Circular!)\r\n", 42, 100);
    }
    
    if(huart1.hdmatx->Init.Direction == DMA_MEMORY_TO_PERIPH) {
        HAL_UART_Transmit(&huart1, (uint8_t*)"? DMA Direction: Memory to Peripheral\r\n", 39, 100);
    }
    
    if(huart1.hdmatx->Init.MemInc == DMA_MINC_ENABLE) {
        HAL_UART_Transmit(&huart1, (uint8_t*)"? Memory Address Increment: Enabled\r\n", 38, 100);
    }
    
    HAL_UART_Transmit(&huart1, (uint8_t*)"DMA check completed.\r\n\r\n", 24, 100);
}

/**
  * @brief 发送状态信息
  */
void Send_Status_Info(void)
{
    char status_msg[120];
    uint32_t total_transfers = dma_half_transfer_count + dma_full_transfer_count;
    uint32_t total_bytes = total_transfers * DMA_BUFFER_SIZE;
    
    sprintf(status_msg, 
            "[STATUS] Half:%lu | Full:%lu | Total:%lu | Bytes:%lu\r\n", 
            dma_half_transfer_count, 
            dma_full_transfer_count, 
            total_transfers, 
            total_bytes);
    
    // 使用短超时发送，避免阻塞
    HAL_UART_Transmit(&huart1, (uint8_t*)status_msg, strlen(status_msg), 10);
}

/**
  * @brief DMA传输一半完成回调函数
  */
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        dma_half_transfer_count++;
        
        // 可选：在这里可以更新前半部分缓冲区数据
        // 用于实现双缓冲机制
    }
}

/**
  * @brief DMA传输完成回调函数
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        dma_full_transfer_count++;
        
        // 注意：在循环模式下，DMA会自动重新开始传输
        // 不需要在这里重新启动DMA
    }
}

/**
  * @brief DMA传输错误回调函数
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uint32_t error_code = HAL_UART_GetError(huart);
        
        char error_msg[60];
        sprintf(error_msg, "DMA Error: 0x%08lX. Restarting...\r\n", error_code);
        HAL_UART_Transmit(&huart1, (uint8_t*)error_msg, strlen(error_msg), 100);
        
        // 停止当前DMA传输
        HAL_UART_DMAStop(&huart1);
        
        // 重新启动DMA传输
        HAL_UART_Transmit_DMA(&huart1, dma_buffer, DMA_BUFFER_SIZE);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/