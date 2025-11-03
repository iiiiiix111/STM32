#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <includes.h>

#define LED1_TASK_PRIO      5
#define LED2_TASK_PRIO      6
#define UART_TASK_PRIO      7
#define LED1_TASK_STK_SIZE  128
#define LED2_TASK_STK_SIZE  128
#define UART_TASK_STK_SIZE  128
#define APP_TASK_START_STK_SIZE 128

static OS_TCB AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_TCB Led1TaskTCB;
static CPU_STK Led1TaskStk[LED1_TASK_STK_SIZE];
static OS_TCB Led2TaskTCB;
static CPU_STK Led2TaskStk[LED2_TASK_STK_SIZE];
static OS_TCB UartTaskTCB;
static CPU_STK UartTaskStk[UART_TASK_STK_SIZE];

static void AppTaskCreate(void);
static void AppObjCreate(void);
static void AppTaskStart(void *p_arg);
static void Led1Task(void *p_arg);
static void Led2Task(void *p_arg);
static void UartTask(void *p_arg);

void SystemClock_Config(void);

int main(void)
{
    OS_ERR err;
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();  // 串口初始化，一定要加上

    OSInit(&err);

    OSTaskCreate(&AppTaskStartTCB,
                 "App Task Start",
                 AppTaskStart,
                 0,
                 LED1_TASK_PRIO - 1,
                 &AppTaskStartStk[0],
                 16, APP_TASK_START_STK_SIZE,
                 0, 0, 0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);

    OSStart(&err);
}

static void AppTaskStart(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    AppTaskCreate();
    AppObjCreate();
    while (1) {
        OSTimeDly(100, OS_OPT_TIME_DLY, &err);
    }
}

static void AppTaskCreate(void)
{
    OS_ERR err;
    OSTaskCreate(&Led1TaskTCB,
                 "Led1 Task",
                 Led1Task,
                 0,
                 LED1_TASK_PRIO,
                 &Led1TaskStk[0],
                 16, LED1_TASK_STK_SIZE,
                 0, 0, 0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
    OSTaskCreate(&Led2TaskTCB,
                 "Led2 Task",
                 Led2Task,
                 0,
                 LED2_TASK_PRIO,
                 &Led2TaskStk[0],
                 16, LED2_TASK_STK_SIZE,
                 0, 0, 0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
    OSTaskCreate(&UartTaskTCB,
                 "Uart Task",
                 UartTask,
                 0,
                 UART_TASK_PRIO,
                 &UartTaskStk[0],
                 16, UART_TASK_STK_SIZE,
                 0, 0, 0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
}

static void AppObjCreate(void) {}

// LED1：1秒周期闪烁
static void Led1Task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    while (1)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}
// LED2：3秒周期闪烁
static void Led2Task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    while (1)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        OSTimeDly(3000, OS_OPT_TIME_DLY, &err);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        OSTimeDly(3000, OS_OPT_TIME_DLY, &err);
    }
}
// 串口发送：2秒周期
static void UartTask(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    char msg[] = "hello uc/OS! 欢迎来到RTOS多任务环境！\r\n";
    while(1)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg)-1, 100);
        OSTimeDly(2000, OS_OPT_TIME_DLY, &err);
    }
}

/* 必须有的外部变量和初始化函数声明 */
extern UART_HandleTypeDef huart1;
// SystemClock_Config/MX_GPIO_Init/MX_USART1_UART_Init函数要在stm32cube生成并链接

void Error_Handler(void) {}
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif
/* USER CODE BEGIN 4 */
// 添加 SystemClock_Config 函数实现
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
/* USER CODE END 4 */