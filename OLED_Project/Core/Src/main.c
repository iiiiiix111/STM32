#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
uint8_t OLED_Test_Communication(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  // 最基础的测试 - 不依赖任何复杂逻辑
  uint8_t test_msg[] = "!!! 基础硬件测试 !!!\r\n";
  HAL_UART_Transmit(&huart1, test_msg, sizeof(test_msg)-1, 1000);
  
  // 测试LED
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  uint8_t led_msg[] = "LED应该亮了\r\n";
  HAL_UART_Transmit(&huart1, led_msg, sizeof(led_msg)-1, 1000);
  HAL_Delay(1000);
  
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  uint8_t led_off[] = "LED应该灭了\r\n";
  HAL_UART_Transmit(&huart1, led_off, sizeof(led_off)-1, 1000);
  HAL_Delay(1000);
  
  // 测试I2C扫描
  uint8_t scan_msg[] = "开始I2C设备扫描...\r\n";
  HAL_UART_Transmit(&huart1, scan_msg, sizeof(scan_msg)-1, 1000);
  
  uint8_t found = 0;
  for(uint8_t addr = 1; addr < 127; addr++)
  {
      if(HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 3, 10) == HAL_OK)
      {
          uint8_t device_msg[30];
          int len = sprintf((char*)device_msg, "发现设备: 0x%02X\r\n", addr);
          HAL_UART_Transmit(&huart1, device_msg, len, 1000);
          found++;
      }
  }
  
  if(found == 0)
  {
      uint8_t no_devices[] = "!!! 未发现任何I2C设备 !!!\r\n";
      HAL_UART_Transmit(&huart1, no_devices, sizeof(no_devices)-1, 1000);
  }
  else
  {
      uint8_t found_msg[30];
      int len = sprintf((char*)found_msg, "扫描完成，发现 %d 个设备\r\n", found);
      HAL_UART_Transmit(&huart1, found_msg, len, 1000);
  }
  
  // OLED专用测试
  uint8_t oled_test_msg[] = "\r\n=== OLED通信测试 ===\r\n";
  HAL_UART_Transmit(&huart1, oled_test_msg, sizeof(oled_test_msg)-1, 1000);
  
  uint8_t oled_result = OLED_Test_Communication();
  if(oled_result == 0)
  {
      uint8_t oled_ok[] = "? OLED通信正常！\r\n";
      HAL_UART_Transmit(&huart1, oled_ok, sizeof(oled_ok)-1, 1000);
  }
  else
  {
      uint8_t oled_fail[60];
      int len = sprintf((char*)oled_fail, "? OLED通信失败！错误代码: %d\r\n", oled_result);
      HAL_UART_Transmit(&huart1, oled_fail, len, 1000);
      
      // 根据错误代码给出建议
      if(oled_result == 1)
      {
          uint8_t suggestion[] = "建议: 尝试I2C地址 0x3D\r\n";
          HAL_UART_Transmit(&huart1, suggestion, sizeof(suggestion)-1, 1000);
      }
      else if(oled_result == 2)
      {
          uint8_t suggestion[] = "建议: 检查电源和上拉电阻\r\n";
          HAL_UART_Transmit(&huart1, suggestion, sizeof(suggestion)-1, 1000);
      }
  }
  
  uint8_t ready_msg[] = "系统就绪，进入主循环\r\n";
  HAL_UART_Transmit(&huart1, ready_msg, sizeof(ready_msg)-1, 1000);
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  uint32_t counter = 0;
  while (1)
  {
    // 简单的心跳信号
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    
    uint8_t heartbeat[40];
    int len = sprintf((char*)heartbeat, "心跳: %lu, OLED状态: %s\r\n", 
                     counter++, 
                     (OLED_Test_Communication() == 0) ? "正常" : "异常");
    HAL_UART_Transmit(&huart1, heartbeat, len, 1000);
    
    HAL_Delay(1000);
  }
  /* USER CODE END WHILE */
}

// OLED通信测试函数
uint8_t OLED_Test_Communication(void)
{
    uint8_t oled_addresses[] = {0x3C, 0x3D, 0x78, 0x7A};
    char* address_names[] = {"0x3C", "0x3D", "0x78", "0x7A"};
    
    uint8_t found = 0;
    
    // 先扫描所有地址，显示详细结果
    uint8_t scan_msg[] = "详细扫描OLED地址:\r\n";
    HAL_UART_Transmit(&huart1, scan_msg, sizeof(scan_msg)-1, 1000);
    
    for(int i = 0; i < 4; i++)
    {
        uint8_t msg[60];
        HAL_I2C_StateTypeDef i2c_state = HAL_I2C_GetState(&hi2c1);
        
        int len = sprintf((char*)msg, "测试地址 %s (0x%02X)... ", address_names[i], oled_addresses[i]);
        HAL_UART_Transmit(&huart1, msg, len, 1000);
        
        HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(&hi2c1, oled_addresses[i], 3, 10);
        
        if(result == HAL_OK)
        {
            uint8_t found_msg[] = "设备响应 ?\r\n";
            HAL_UART_Transmit(&huart1, found_msg, sizeof(found_msg)-1, 1000);
            found = 1;
            
            // 测试发送命令
            uint8_t test_cmd = 0xAE; // 关闭显示
            if(HAL_I2C_Master_Transmit(&hi2c1, oled_addresses[i], &test_cmd, 1, 100) == HAL_OK)
            {
                uint8_t cmd_msg[] = "  命令发送成功 ?\r\n";
                HAL_UART_Transmit(&huart1, cmd_msg, sizeof(cmd_msg)-1, 1000);
            }
            else
            {
                uint8_t cmd_msg[] = "  命令发送失败 ?\r\n";
                HAL_UART_Transmit(&huart1, cmd_msg, sizeof(cmd_msg)-1, 1000);
            }
            break;
        }
        else
        {
            uint8_t not_found_msg[] = "无响应 ?\r\n";
            HAL_UART_Transmit(&huart1, not_found_msg, sizeof(not_found_msg)-1, 1000);
        }
    }
    
    if(!found)
    {
        // 提供详细的故障排除指南
        uint8_t trouble_msg[] = "\r\n!!! OLED连接问题 !!!\r\n";
        HAL_UART_Transmit(&huart1, trouble_msg, sizeof(trouble_msg)-1, 1000);
        
        uint8_t checklist[] = "请检查:\r\n";
        HAL_UART_Transmit(&huart1, checklist, sizeof(checklist)-1, 1000);
        
        uint8_t item1[] = "1. ? OLED VCC 连接到 3.3V (不是5V!)\r\n";
        HAL_UART_Transmit(&huart1, item1, sizeof(item1)-1, 1000);
        
        uint8_t item2[] = "2. ? OLED GND 连接到 GND\r\n";
        HAL_UART_Transmit(&huart1, item2, sizeof(item2)-1, 1000);
        
        uint8_t item3[] = "3. ? OLED SDA 连接到 PB7\r\n";
        HAL_UART_Transmit(&huart1, item3, sizeof(item3)-1, 1000);
        
        uint8_t item4[] = "4. ? OLED SCL 连接到 PB6\r\n";
        HAL_UART_Transmit(&huart1, item4, sizeof(item4)-1, 1000);
        
        uint8_t item5[] = "5. ? I2C总线是否有4.7kΩ上拉电阻?\r\n";
        HAL_UART_Transmit(&huart1, item5, sizeof(item5)-1, 1000);
        
        uint8_t item6[] = "6. ? OLED模块电源指示灯是否亮?\r\n";
        HAL_UART_Transmit(&huart1, item6, sizeof(item6)-1, 1000);
        
        uint8_t item7[] = "7. ? 尝试另一个OLED模块?\r\n";
        HAL_UART_Transmit(&huart1, item7, sizeof(item7)-1, 1000);
        
        return 1;
    }
    
    return 0;
}

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