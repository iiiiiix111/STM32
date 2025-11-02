/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_u8g2.h"
#include "test.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8g2_t u8g2;
uint32_t lastDisplayTick = 0;
uint8_t displayState = 0;
uint8_t animationFrame = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void displayStudentInfo(void);
void displaySlidingEffects(void);
void displayAnimation(void);
void drawRotatingSquare(u8g2_t *u8g2, uint8_t frame);
void drawBouncingBall(u8g2_t *u8g2, uint8_t frame);
void drawBlinkingSmiley(u8g2_t *u8g2, uint8_t frame);
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
  MX_I2C2_Init();
  MX_TIM1_Init();
  
  /* USER CODE BEGIN 2 */
  // 初始化U8g2
  u8g2Init(&u8g2);
  
  // 开启TIM1用于动画定时
  HAL_TIM_Base_Start(&htim1);
  
  // 初始显示欢迎信息
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_ncenB12_tf);
  u8g2_DrawStr(&u8g2, 10, 30, "OLED Ready!");
  u8g2_SendBuffer(&u8g2);
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t currentTick = HAL_GetTick();
    
    // 每2秒切换显示状态
    if (currentTick - lastDisplayTick > 2000) {
      lastDisplayTick = currentTick;
      displayState = (displayState + 1) % 3;
      animationFrame = 0;
    }
    
    // 根据显示状态选择不同的显示内容
    switch (displayState) {
      case 0:
        displayStudentInfo();
        break;
      case 1:
        displaySlidingEffects();
        break;
      case 2:
        displayAnimation();
        break;
    }
    
    // 更新动画帧
    animationFrame++;
    if (animationFrame > 100) animationFrame = 0;
    
    // 控制刷新率
    HAL_Delay(50);
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

// 显示学号和姓名
void displayStudentInfo(void)
{
  u8g2_ClearBuffer(&u8g2);
  
  // 显示学号 - 大字体
  u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tf);
  u8g2_DrawStr(&u8g2, 17, 20, "632307030403");
  
  // 显示姓名 - 中等字体
  u8g2_SetFont(&u8g2, u8g2_font_ncenB12_tf);
  u8g2_DrawStr(&u8g2, 13, 40, "zhengjiabao");
  
  // 显示分隔线
  u8g2_DrawLine(&u8g2, 10, 25, 118, 25);
  
  u8g2_SendBuffer(&u8g2);
}

// 滑动显示效果
void displaySlidingEffects(void)
{
  static uint8_t slidePos = 0;
  static uint8_t slideDir = 0; // 0:右到左, 1:左到右
  static uint8_t slideCount = 0;
  
  u8g2_ClearBuffer(&u8g2);
  
  if (slideDir == 0) {
    // 从右向左滑动
    u8g2_SetFont(&u8g2, u8g2_font_ncenB12_tf);
    u8g2_DrawStr(&u8g2, 128 - slidePos, 30, "Sliding Text");
    
    slidePos += 2;
    if (slidePos > 150) {
      slideDir = 1;
      slidePos = 0;
      slideCount++;
    }
  } else {
    // 从左向右滑动
    u8g2_SetFont(&u8g2, u8g2_font_ncenB12_tf);
    u8g2_DrawStr(&u8g2, -80 + slidePos, 30, "Hello OLED!");
    
    slidePos += 2;
    if (slidePos > 150) {
      slideDir = 0;
      slidePos = 0;
      slideCount++;
    }
  }
  
  // 每完成一次滑动循环切换文本
  if (slideCount >= 2) {
    slideCount = 0;
  }
  
  u8g2_SendBuffer(&u8g2);
}

// 动画显示
void displayAnimation(void)
{
  static uint8_t animType = 0;
  static uint32_t lastAnimChange = 0;
  
  // 每3秒切换动画类型
  if (HAL_GetTick() - lastAnimChange > 3000) {
    lastAnimChange = HAL_GetTick();
    animType = (animType + 1) % 3;
  }
  
  u8g2_ClearBuffer(&u8g2);
  
  switch (animType) {
    case 0:
      drawRotatingSquare(&u8g2, animationFrame);
      u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tf);
      u8g2_DrawStr(&u8g2, 40, 60, "Rotating");
      break;
      
    case 1:
      drawBouncingBall(&u8g2, animationFrame);
      u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tf);
      u8g2_DrawStr(&u8g2, 40, 60, "Bouncing");
      break;
      
    case 2:
      drawBlinkingSmiley(&u8g2, animationFrame);
      u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tf);
      u8g2_DrawStr(&u8g2, 45, 60, "Smiley");
      break;
  }
  
  u8g2_SendBuffer(&u8g2);
}

// 绘制旋转方块
void drawRotatingSquare(u8g2_t *u8g2, uint8_t frame)
{
  int center_x = 64;
  int center_y = 25;
  int size = 12;
  
  // 计算旋转角度
  float angle = frame * 0.1f;
  float cos_a = cos(angle);
  float sin_a = sin(angle);
  
  // 计算四个顶点
  int x1 = center_x + (int)(size * cos_a - size * sin_a);
  int y1 = center_y + (int)(size * sin_a + size * cos_a);
  int x2 = center_x + (int)(-size * cos_a - size * sin_a);
  int y2 = center_y + (int)(-size * sin_a + size * cos_a);
  int x3 = center_x + (int)(-size * cos_a + size * sin_a);
  int y3 = center_y + (int)(-size * sin_a - size * cos_a);
  int x4 = center_x + (int)(size * cos_a + size * sin_a);
  int y4 = center_y + (int)(size * sin_a - size * cos_a);
  
  // 绘制旋转的方块
  u8g2_DrawLine(u8g2, x1, y1, x2, y2);
  u8g2_DrawLine(u8g2, x2, y2, x3, y3);
  u8g2_DrawLine(u8g2, x3, y3, x4, y4);
  u8g2_DrawLine(u8g2, x4, y4, x1, y1);
}

// 绘制弹跳球
void drawBouncingBall(u8g2_t *u8g2, uint8_t frame)
{
  static int ball_y = 10;
  static int velocity = 2;
  
  int ball_x = 32 + (frame % 96);
  int radius = 6;
  
  // 更新球的位置和速度
  ball_y += velocity;
  
  // 碰撞检测
  if (ball_y + radius > 40) {
    ball_y = 40 - radius;
    velocity = -velocity;
  }
  if (ball_y - radius < 10) {
    ball_y = 10 + radius;
    velocity = -velocity;
  }
  
  // 绘制球
  u8g2_DrawDisc(u8g2, ball_x, ball_y, radius, U8G2_DRAW_ALL);
  
  // 绘制边界
  u8g2_DrawFrame(u8g2, 20, 5, 88, 40);
}

// 绘制眨眼笑脸
void drawBlinkingSmiley(u8g2_t *u8g2, uint8_t frame)
{
  int center_x = 64;
  int center_y = 25;
  int radius = 15;
  
  // 绘制脸轮廓
  u8g2_DrawCircle(u8g2, center_x, center_y, radius, U8G2_DRAW_ALL);
  
  // 眼睛 - 实现眨眼效果
  if ((frame / 20) % 2 == 0) {
    // 正常眼睛
    u8g2_DrawDisc(u8g2, center_x - 6, center_y - 3, 3, U8G2_DRAW_ALL);
    u8g2_DrawDisc(u8g2, center_x + 6, center_y - 3, 3, U8G2_DRAW_ALL);
  } else {
    // 眨眼状态
    u8g2_DrawLine(u8g2, center_x - 9, center_y - 3, center_x - 3, center_y - 3);
    u8g2_DrawLine(u8g2, center_x + 9, center_y - 3, center_x + 3, center_y - 3);
  }
  
  // 微笑的嘴巴
  u8g2_DrawDisc(u8g2, center_x, center_y + 5, 8, U8G2_DRAW_UPPER_RIGHT);
  u8g2_DrawDisc(u8g2, center_x, center_y + 5, 8, U8G2_DRAW_UPPER_LEFT);
}

// 如果需要运行原有的测试函数，可以取消注释下面的代码
/*
void runOriginalTests(void)
{
    u8g2DrawTest(&u8g2);
}
*/

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
    // 错误时快速闪烁LED
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(100);
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