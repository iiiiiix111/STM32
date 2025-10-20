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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 呼吸灯结构体定义
typedef struct {
    int direction;      // 方向：1=增加，-1=减少
    uint32_t duty;      // 当前占空比
    uint32_t max_duty;  // 最大占空比
    uint32_t step;      // 变化步长
} BreathLED_t;

// 正弦波呼吸灯结构体
typedef struct {
    float angle;        // 当前角度
    float increment;    // 角度增量
    uint32_t max_duty;  // 最大占空比
} SineBreathLED_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 选择呼吸效果模式
#define USE_SINE_BREATH 1  // 1=使用正弦波，0=使用线性变化
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#if USE_SINE_BREATH
// 正弦波呼吸灯参数 - 大幅降低速度
SineBreathLED_t sine_led1 = {0.0f, 0.008f, 1000};    // TIM3 LED1 - 很慢
SineBreathLED_t sine_led2 = {0.0f, 0.012f, 1000};    // TIM4 LED2 - 慢速
SineBreathLED_t sine_led3 = {0.0f, 0.020f, 100};     // 板载LED - 中速
#else
// 线性呼吸灯参数 - 减小步长
BreathLED_t linear_led1 = {1, 0, 1000, 1};    // TIM3 LED1 - 小步长
BreathLED_t linear_led2 = {1, 0, 1000, 2};    // TIM4 LED2 - 中等步长  
BreathLED_t linear_led3 = {1, 0, 100, 1};     // 板载LED - 小步长
#endif

uint32_t pwm_counter = 0;      // 软件PWM计数器
uint8_t update_count = 0;      // 硬件PWM更新计数器
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint32_t UpdateLinearBreathLED(BreathLED_t *led);
uint32_t UpdateSineBreathLED(SineBreathLED_t *led);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  
  // 启动定时器
  HAL_TIM_Base_Start_IT(&htim2);          // TIM2用于软件PWM中断
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // TIM3硬件PWM
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // TIM4硬件PWM
  
  // 初始PWM占空比设置为0（LED最暗）
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
  
  // 板载LED初始状态为熄灭
  HAL_GPIO_WritePin(LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin, GPIO_PIN_RESET);
  
  // 初始化变量
  pwm_counter = 0;
  update_count = 0;
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 主循环空闲处理
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief 线性呼吸灯更新函数
  */
uint32_t UpdateLinearBreathLED(BreathLED_t *led)
{
    if (led->direction > 0) {
        led->duty += led->step;
        if (led->duty >= led->max_duty) {
            led->duty = led->max_duty;
            led->direction = -1;
        }
    } else {
        if (led->duty > led->step) {
            led->duty -= led->step;
        } else {
            led->duty = 0;
            led->direction = 1;
        }
    }
    return led->duty;
}

/**
  * @brief 正弦波呼吸灯更新函数
  */
uint32_t UpdateSineBreathLED(SineBreathLED_t *led)
{
    led->angle += led->increment;
    if (led->angle > 2 * M_PI) {
        led->angle -= 2 * M_PI;
    }
    
    float sine_val = (sinf(led->angle) + 1.0f) / 2.0f;
    return (uint32_t)(sine_val * led->max_duty);
}

/**
  * @brief 定时器中断回调函数
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) 
    {
        // 软件PWM实现 - 用于板载LED
        pwm_counter++;
        if (pwm_counter >= 200) {  // 增加软件PWM周期到200，降低频率
            pwm_counter = 0;
        }
        
        uint32_t onboard_duty;
        
        #if USE_SINE_BREATH
        // 正弦波呼吸效果
        onboard_duty = UpdateSineBreathLED(&sine_led3);
        #else
        // 线性呼吸效果  
        onboard_duty = UpdateLinearBreathLED(&linear_led3);
        #endif
        
        // 控制板载LED (PC13)
        if (pwm_counter < onboard_duty) {
            HAL_GPIO_WritePin(LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin, GPIO_PIN_RESET);
        }
        
        // 大幅降低硬件PWM更新频率
        if (++update_count >= 20) {  // 每20次中断更新一次硬件PWM（很慢）
            update_count = 0;
            
            uint32_t led1_duty, led2_duty;
            
            #if USE_SINE_BREATH
            // 正弦波呼吸效果
            led1_duty = UpdateSineBreathLED(&sine_led1);  // TIM3 LED1
            led2_duty = UpdateSineBreathLED(&sine_led2);  // TIM4 LED2
            #else
            // 线性呼吸效果
            led1_duty = UpdateLinearBreathLED(&linear_led1);  // TIM3 LED1
            led2_duty = UpdateLinearBreathLED(&linear_led2);  // TIM4 LED2
            #endif
            
            // 更新硬件PWM占空比
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, led1_duty);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, led2_duty);
        }
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
