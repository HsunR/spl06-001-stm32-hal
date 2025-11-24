#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "spl06.h"

/* UART配置定义 */
UART_HandleTypeDef huart1;

/* 函数声明 */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void Error_Handler(void);

/* printf重定向函数 */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

int main(void)
{
  /* HAL初始化 */
  HAL_Init();
  
  /* 配置系统时钟 */
  SystemClock_Config();

  /* 初始化所有配置的外设 */
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  printf("SPL06传感器测试程序启动\r\n");

  /* 初始化SPL06传感器 */
  if (spl06_init() != 0)
  {
    printf("SPL06传感器初始化失败\r\n");
    while (1)
    {
      HAL_Delay(1000);
    }
  }
  printf("SPL06传感器初始化成功\r\n");

  /* 主循环 */
  while (1)
  {
    /* 更新传感器数据 */
    spl06_update();
    
    /* 获取温度和压力值 */
    float temperature = spl06_get_temperature();
    float pressure = spl06_get_pressure();
    
    /* 计算海拔高度（可选） - 基于标准大气模型 */
    float altitude = 44330.0f * (1.0f - pow(pressure / 101325.0f, 0.1903f));
    
    /* 打印测量数据 */
    printf("温度: %.2f °C, 压力: %.2f Pa, 海拔: %.2f m\r\n", 
           temperature, pressure, altitude);
    
    /* 延时1秒 */
    HAL_Delay(1000);
  }
}

/**
  * @brief 系统时钟配置
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 配置主振荡器
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /** 配置CPU, AHB和APB时钟
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1初始化函数
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO初始化函数
  */
static void MX_GPIO_Init(void)
{
  /* 此处可以根据实际硬件配置GPIO */
  /* 例如LED灯引脚、按键引脚等 */
}

/**
  * @brief 错误处理函数
  */
void Error_Handler(void)
{
  /* 无限循环 */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief 断言失败时调用的函数
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  printf("错误: 文件 %s 第 %ld 行\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */