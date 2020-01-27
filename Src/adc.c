/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
int g_MeasurementNumber = 0;
/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA2_Stream4;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_adc1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_adc1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_adc1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	uint32_t g_sum = 0;
	for (int i = 0; i<ADC_BUFFER_LENGTH; i++)
		g_sum += g_ADCBuffer[i];

	uint32_t adc = g_sum/ADC_BUFFER_LENGTH;
	g_MeasurementNumber += ADC_BUFFER_LENGTH;

	char display_string[30] = {'0'};
	sprintf(display_string, "ADC Value: %lu", adc);
	HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);
	//ILI9341_Draw_Text(display_string, 10, 10, YELLOW, 2, BLACK);
	ILI9341_Draw_String(20, 20, WHITE, BLACK, display_string, 2);

	float voltage = (float)adc * 3.3f/4096.0f;
	sprintf(display_string, "Voltage: %.2f V", voltage);
	HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);
	ILI9341_Draw_String(20, 40, WHITE, BLACK, display_string, 2);

	/* Temp. sensor characteristics from the STM32F4 datasheet:
	 *  Slope: 2.5mV/°C
	 *  Voltage at 25°C = 0.76
	 *  What means that: V = 760 mV + 2.5*(T - 25°C) = 697.5 mV + 2.5*T
	 *  Back calculating: T = (V - 697.5)/2.5
	 */
	float temperature = (voltage*1000 - 697.5) / 2.5;
	sprintf(display_string, "Temperature: %.2f", temperature);
	HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);
	ILI9341_Draw_String(20, 60, WHITE, BLACK, display_string, 2);

	char err_msg[40];
	sprintf(err_msg, "ADC State: 0x%lX\n", HAL_ADC_GetState(hadc));
	HAL_UART_Transmit(&huart2, (uint8_t*) err_msg, strlen(err_msg), 0xFFFF);

}


void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc){

	char err_msg[40];
	sprintf(err_msg, "ADC State: 0x%lX\n", HAL_ADC_GetState(hadc));
	HAL_UART_Transmit(&huart2, (uint8_t*) err_msg, strlen(err_msg), 0xFFFF);
	HAL_ADC_Start_DMA(hadc, g_ADCBuffer, ADC_BUFFER_LENGTH);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
