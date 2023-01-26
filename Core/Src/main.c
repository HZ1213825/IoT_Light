/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Delay.h"
#include "IR_NEC.h"
#include "RF.h"
#include "Steering_Engine.h"
#include "Print.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t Input_EN = 1;
uint8_t M_EN = 0;
uint8_t Uart1_Buf[20] = {0};
uint8_t Uart1_OK = 0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void OPEN()
{
  M_EN = 1;
  HAL_IWDG_Refresh(&hiwdg);
  Steering_Engine_360(0, 30);

  HAL_Delay(500);
  HAL_IWDG_Refresh(&hiwdg);
  Steering_Engine_360(1, 40);
  HAL_Delay(80);
  HAL_IWDG_Refresh(&hiwdg);
  Steering_Engine_Stop();
  M_EN = 0;
}
void CLOSE()
{
  M_EN = 1;
  HAL_IWDG_Refresh(&hiwdg);
  Steering_Engine_360(1, 30);
  HAL_Delay(500);
  HAL_IWDG_Refresh(&hiwdg);
  Steering_Engine_360(0, 30);
  HAL_Delay(80);
  HAL_IWDG_Refresh(&hiwdg);
  Steering_Engine_Stop();
  M_EN = 0;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim1)
  {
  }
  else if (htim == &htim2)
  {
    if (M_EN == 1)
      Steering_Engine_Action();
    else
      HAL_GPIO_WritePin(Steering_Engine_GPIOx, Steering_Engine_GPIO_Pin, GPIO_PIN_SET);
  }
  else if (htim == &htim3)
  {
  }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_2) // 433
  {
    if (Input_EN == 1)
      if (IR_NEC_Read_ins == 0)
        if (RF_READ_OK == 0)
          RF_Read_Decode();
  }
  else if (GPIO_Pin == GPIO_PIN_3) // IR
  {
    if (Input_EN == 1)
      if (IR_NEC_Read_OK == 0)
        IR_NEC_Read_Decode(air);
  }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart1)
  {
    Uart1_OK = 1;
    HAL_UART_Transmit(&huart2, Uart1_Buf, Size, 0xfff);
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  // uint8_t Data_home_Close[3] = {0xac, 0x00, 0xFF};
  // uint8_t Data_home_Open[3] = {0xac, 0x00, 0x00};

  uint8_t Data_N_Close[3] = {0xac, 0x11, 0xFF};
  uint8_t Data_N_Open[3] = {0xac, 0x11, 0x00};

  uint8_t Data_S_Close[3] = {0xac, 0x22, 0xFF};
  uint8_t Data_S_Open[3] = {0xac, 0x22, 0x00};
  uint8_t RF433_Buf[3] = {0};
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
	MX_IWDG_Init();
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(50);
  printf("1\r\n");
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, Uart1_Buf, 20);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if (RF_READ_OK == 1)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
      Input_EN = 0;
      if (RF_READ_data[0] == 0xac && RF_READ_data[1] == 0x01 && RF_READ_data[2] == 0x00)
        OPEN();
      else if (RF_READ_data[0] == 0xac && RF_READ_data[1] == 0x01 && RF_READ_data[2] == 0xff)
        CLOSE();
      HAL_IWDG_Refresh(&hiwdg);
      RF_READ_data[0] = 0;
      RF_READ_data[1] = 0;
      RF_READ_data[2] = 0;
      RF_READ_OK = 0;
      Input_EN = 1;
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    if (IR_NEC_Read_OK == 1)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
      Input_EN = 0;
      //  printf("%02X%02X%02X\r\n", RF_READ_data[0], RF_READ_data[1], RF_READ_data[2]);

      if (IR_NEC_Read_Dat[0] == 0x4D && IR_NEC_Read_Dat[1] == 0xb2 && IR_NEC_Read_Dat[2] == 0xa3 && IR_NEC_Read_Dat[3] == 0x5C)
        OPEN();
      else if (IR_NEC_Read_Dat[0] == 0x4D && IR_NEC_Read_Dat[1] == 0xb2 && IR_NEC_Read_Dat[2] == 0x59 && IR_NEC_Read_Dat[3] == 0xa6)
        CLOSE();

      if (IR_NEC_Read_Dat[0] == 0x84 && IR_NEC_Read_Dat[1] == 0xff && IR_NEC_Read_Dat[2] == 0x81 && IR_NEC_Read_Dat[3] == 0x7e)
        OPEN();
      else if (IR_NEC_Read_Dat[0] == 0x84 && IR_NEC_Read_Dat[1] == 0xff && IR_NEC_Read_Dat[2] == 0x01 && IR_NEC_Read_Dat[3] == 0xfe)
        CLOSE();

      HAL_IWDG_Refresh(&hiwdg);
      IR_NEC_Read_Dat[0] = 0;
      IR_NEC_Read_Dat[1] = 0;
      IR_NEC_Read_Dat[2] = 0;
      IR_NEC_Read_Dat[3] = 0;
      IR_NEC_Read_OK = 0;
      Input_EN = 1;
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    if (Uart1_OK == 1)
    {
      Input_EN = 0;
      HAL_IWDG_Refresh(&hiwdg);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
      if (Uart1_Buf[0] == 0xac && Uart1_Buf[1] != 0x00 && Uart1_Buf[1] != 0xFF)
      {
        for (int i = 0; i < 3; i++)
          RF433_Buf[i] = Uart1_Buf[i];
        RF_Write_Send(RF433_Buf);
      }
      else if (Uart1_Buf[0] == 0xac && Uart1_Buf[1] == 0x00)
      {
        if (Uart1_Buf[2] == 0x00)
          OPEN();
        else if (Uart1_Buf[2] == 0xff)
          CLOSE();
      }
      if (Uart1_Buf[0] == 0xac && Uart1_Buf[1] == 0xff)
      {
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        if (Uart1_Buf[2] == 0x00)
        {
          RF_Write_Send(Data_N_Open);
          HAL_IWDG_Refresh(&hiwdg);
          HAL_Delay(300);
          HAL_IWDG_Refresh(&hiwdg);
          RF_Write_Send(Data_S_Open);
          OPEN();
        }
        else if (Uart1_Buf[2] == 0xff)
        {
          RF_Write_Send(Data_N_Close);
          HAL_IWDG_Refresh(&hiwdg);
          HAL_Delay(300);
          HAL_IWDG_Refresh(&hiwdg);
          RF_Write_Send(Data_S_Close);
          HAL_IWDG_Refresh(&hiwdg);
          CLOSE();
        }
      }
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
      Input_EN = 1;
      Uart1_OK = 0;
      HAL_UARTEx_ReceiveToIdle_IT(&huart1, Uart1_Buf, 20);
    }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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
