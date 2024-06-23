/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "MAX31855.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
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
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

int section_height = ILI9341_SCREEN_WIDTH/4;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int time_left_current_stage;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//	Check which timer trigered the callback
	if (htim == &htim4) {
		if (time_left_current_stage > 0) {
			time_left_current_stage--;
		}
	}
}

void run_profile_section (struct s_profile_section profile_section, char section_title[7]) {
	time_left_current_stage = profile_section.time;
  ILI9341_FillScreen(BLACK);

//    	  Display the title block for the preheat screen
  char preheat_title[32];
	ILI9341_DrawRectangle(0, 0*section_height, ILI9341_SCREEN_HEIGHT, section_height, profile_section.bg_colour);
	sprintf(preheat_title, "%s - Target %dC", section_title, profile_section.temperature);
	ILI9341_DrawText (preheat_title, FONT4, 20, 0*section_height+(section_height-19)/2, profile_section.txt_colour, profile_section.bg_colour);

	while (time_left_current_stage > 0) {
		char remaining_time_str[8];
		sprintf(remaining_time_str, "%02i:%02i", (int) floor(time_left_current_stage/60), time_left_current_stage%60);
		ILI9341_DrawText (remaining_time_str, FONT4, 80, 100, WHITE, BLACK);
		char temp_str [32];
		HAL_Delay(250);
		MAX31855_Data data = MAX31855_ReadData();
	  if (data.fault == MAX31855_NO_FAULT) {
		  snprintf(temp_str, sizeof temp_str, "%.2f", data.temperature);
	  } else {
	  	snprintf(temp_str, sizeof temp_str, "Fault - %i", data.fault);
		}
	  ILI9341_DrawText(temp_str, FONT4, 80, 150, WHITE, BLACK);
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
	enum e_state current_state;
	current_state = splash;


	struct s_profile profile;
	profile.preheat.temperature = 180;
	profile.preheat.time = 20;
	profile.preheat.bg_colour = RED;
	profile.preheat.txt_colour = WHITE;
	profile.soak.temperature = 150;
	profile.soak.time = 15;
	profile.soak.bg_colour = ORANGE;
	profile.soak.txt_colour = WHITE;
	profile.reflow.temperature = 230;
	profile.reflow.time = 10;
	profile.reflow.bg_colour = CYAN;
	profile.reflow.txt_colour = BLACK;

	bool drawn = false;


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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	ILI9341_Init();

	HAL_TIM_Base_Start_IT(&htim4);

//	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  	// Draw the screens based on the current_state
  	if (current_state == splash) {
//  		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
  	  ILI9341_FillScreen(BLACK);
  	  ILI9341_DrawText("Solder Reflow Oven", FONT4, 5, 110, WHITE, BLACK);
  	  HAL_Delay(1000);
  	  current_state = homepage;
  	  drawn = false;
  	} else if (current_state == homepage) {
//  		Show the current profile on the home screen
  		if (!drawn) {
    	  ILI9341_FillScreen(BLACK);
				ILI9341_DrawRectangle(0, 0*section_height, ILI9341_SCREEN_HEIGHT, section_height, profile.preheat.bg_colour);
				ILI9341_DrawRectangle(0, 1*section_height, ILI9341_SCREEN_HEIGHT, section_height, profile.soak.bg_colour);
				ILI9341_DrawRectangle(0, 2*section_height, ILI9341_SCREEN_HEIGHT, section_height, profile.reflow.bg_colour);
//				Titles
				ILI9341_DrawText ("Preheat", FONT4, 20, 0*section_height+(section_height-19)/2, profile.preheat.txt_colour, profile.preheat.bg_colour);
				ILI9341_DrawText ("Soak", FONT4, 20, 1*section_height+(section_height-19)/2, profile.soak.txt_colour, profile.soak.bg_colour);
				ILI9341_DrawText ("Reflow", FONT4, 20, 2*section_height+(section_height-19)/2, profile.reflow.txt_colour, profile.reflow.bg_colour);

//				Temps
				char preheat_temp[5];
				sprintf(preheat_temp, "%3dC", profile.preheat.temperature);
				ILI9341_DrawText (preheat_temp, FONT4, 100, 0*section_height+(section_height-19)/2, profile.preheat.txt_colour, profile.preheat.bg_colour);
				char soak_temp[5];
				sprintf(soak_temp, "%3dC", profile.soak.temperature);
				ILI9341_DrawText (soak_temp, FONT4, 100, 1*section_height+(section_height-19)/2, profile.soak.txt_colour, profile.soak.bg_colour);
				char reflow_temp[5];
				sprintf(reflow_temp, "%3dC", profile.reflow.temperature);
				ILI9341_DrawText (reflow_temp, FONT4, 100, 2*section_height+(section_height-19)/2, profile.reflow.txt_colour, profile.reflow.bg_colour);

//				Times
				char preheat_time[8];
				sprintf(preheat_time, "%02i:%02i", (int) floor(profile.preheat.time/60), profile.preheat.time%60);
				ILI9341_DrawText (preheat_time, FONT4, 180, 0*section_height+(section_height-19)/2, profile.preheat.txt_colour, profile.preheat.bg_colour);
				char soak_time[8];
				sprintf(soak_time, "%02i:%02i", (int) floor(profile.soak.time/60), profile.soak.time%60);
				ILI9341_DrawText (soak_time, FONT4, 180, 1*section_height+(section_height-19)/2, profile.soak.txt_colour, profile.soak.bg_colour);
				char reflow_time[8];
				sprintf(reflow_time, "%02i:%02i", (int) floor(profile.reflow.time/60), profile.reflow.time%60);
				ILI9341_DrawText (reflow_time, FONT4, 180, 2*section_height+(section_height-19)/2, profile.reflow.txt_colour, profile.reflow.bg_colour);

				drawn = true;

//				Debug to switch to next screen - will use a button in due course
				HAL_Delay(2000);
				current_state = preheat;
				drawn = false;

  		}
  	}else if (current_state == preheat) {
			run_profile_section(profile.preheat, "Preheat");
			current_state = soak;

  	} else if (current_state == soak) {
			run_profile_section(profile.soak, "Soak");
			current_state = reflow;

  	} else if (current_state == reflow) {
			run_profile_section(profile.reflow, "Reflow");
			current_state = homepage;

  	}else {
  	  ILI9341_FillScreen(WHITE);
  	  HAL_Delay(100);
  	  char str2 [32];
  	  MAX31855_Data data = MAX31855_ReadData();
  	  if (data.fault == MAX31855_NO_FAULT) {
  		  snprintf(str2, sizeof str2, "Int: %.2f Ext: %.2f", data.internalTemperature, data.temperature);
  	  } else {
  	  	snprintf(str2, sizeof str2, "Fault - %i", data.fault);
			}
  	  ILI9341_DrawText(str2, FONT4, 5, 110, BLACK, WHITE);
  	  HAL_Delay(500);
  	}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7200-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
