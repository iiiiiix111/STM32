#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>
#include "aht20.h"
#include "oled.h"  // 添加OLED头文件

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
float temperature = 0.0;
float humidity = 0.0;
char uart_buf[64];
uint32_t last_measure_time = 0;
uint8_t oled_available = 1;  // OLED可用
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
uint8_t OLED_Test(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  // 系统启动信息
  uint8_t startup_msg[] = "\r\n=== AHT20温湿度采集系统 ===\r\n";
  HAL_UART_Transmit(&huart1, startup_msg, sizeof(startup_msg)-1, 1000);
  
  // 测试并初始化OLED
  uint8_t oled_test_msg[] = "初始化OLED显示屏...\r\n";
  HAL_UART_Transmit(&huart1, oled_test_msg, sizeof(oled_test_msg)-1, 1000);
  
  if(OLED_Test() == 0)
  {
      uint8_t oled_ok[] = "? OLED通信正常！\r\n";
      HAL_UART_Transmit(&huart1, oled_ok, sizeof(oled_ok)-1, 1000);
      
      OLED_Init();
      uint8_t oled_init_ok[] = "? OLED初始化完成！\r\n";
      HAL_UART_Transmit(&huart1, oled_init_ok, sizeof(oled_init_ok)-1, 1000);
      
      oled_available = 1;
  }
  else
  {
      uint8_t oled_fail[] = "? OLED通信失败，跳过显示功能\r\n";
      HAL_UART_Transmit(&huart1, oled_fail, sizeof(oled_fail)-1, 1000);
      oled_available = 0;
  }
  
  // 初始化AHT20
  HAL_Delay(100);
  uint8_t init_msg[] = "初始化AHT20传感器...\r\n";
  HAL_UART_Transmit(&huart1, init_msg, sizeof(init_msg)-1, 1000);
  
  if(AHT20_Init() == 0)
  {
      uint8_t success_msg[] = "AHT20初始化成功！\r\n";
      HAL_UART_Transmit(&huart1, success_msg, sizeof(success_msg)-1, 1000);
  }
  else
  {
      uint8_t fail_msg[] = "AHT20初始化失败！\r\n";
      HAL_UART_Transmit(&huart1, fail_msg, sizeof(fail_msg)-1, 1000);
  }
  
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  uint32_t counter = 0;
  while (1)
  {
    // LED闪烁指示程序运行
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    
    // 每隔2秒采集一次温湿度
    if(HAL_GetTick() - last_measure_time >= 2000)
    {
        last_measure_time = HAL_GetTick();
        
        // 采集温湿度
        AHT20_Read_Process(&temperature, &humidity);
        
        // 通过串口输出
        sprintf(uart_buf, "[%lu] 温度:%.2f℃ 湿度:%.2f%%", counter, temperature, humidity);
        
        // OLED显示
        if(oled_available)
        {
            strcat(uart_buf, " [OLED显示]");
            OLED_ShowTempHumi(temperature, humidity);
        }
        else
        {
            strcat(uart_buf, " [无OLED]");
        }
        
        strcat(uart_buf, "\r\n");
        HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, strlen(uart_buf), 1000);
        
        counter++;
    }
    
    HAL_Delay(100);
  }
  /* USER CODE END WHILE */
}

// OLED通信测试
uint8_t OLED_Test(void)
{
    if(HAL_I2C_IsDeviceReady(&hi2c1, 0x78, 3, 10) == HAL_OK)
    {
        uint8_t init_cmd = 0xAE;
        if(HAL_I2C_Master_Transmit(&hi2c1, 0x78, &init_cmd, 1, 100) == HAL_OK)
        {
            return 0;
        }
    }
    return 1;
}

// 保持原有的SystemClock_Config和Error_Handler
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}