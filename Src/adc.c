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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
int g_MeasurementNumber = 0;
/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

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
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration    
    PA0-WKUP     ------> ADC1_IN0 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
  
    /**ADC1 GPIO Configuration    
    PA0-WKUP     ------> ADC1_IN0 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	g_MeasurementNumber += 1;

	uint32_t adc = g_ADCBuffer[0];   // Temperature channel is the first one
	char display_string[30] = {'0'};
	sprintf(display_string, "ADC Value: %lu", adc);
	HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);

	float voltage = (float)adc * 3.3f/4096.0f;
	sprintf(display_string, "Voltage: %.3f V", voltage);
	HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);

	ILI9341_Draw_String(100, 160, WHITE, BLACK, display_string, 2);

	/* Temp. sensor characteristics from the STM32F4 datasheet:
	 *  Slope: 2.5mV/°C
	 *  Voltage at 25°C = 0.76
	 *  What means that: V = 760 mV + 2.5*(T - 25°C) = 697.5 mV + 2.5*T
	 *  Back calculating: T = (V - 697.5)/2.5
	 */

	/*
	 * if Channel == Temperature sensor:
	float temperature = (voltage*1000 - 697.5) / 2.5;
	sprintf(display_string, "Temperature: %.2f", temperature);
	HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);
	ILI9341_Draw_String(20, 60, WHITE, BLACK, display_string, 2);
	*/

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

/* Configure channel based on the input channel number.
 * Pero, Jan.2020
 */
void ADC_ChannelConfig(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

/* Configure and start ADC
 * Pero, Jan.2020
 */
void ADC_ConfigAndRun(char icon){
	HAL_ADC_Stop(&hadc1);
	switch(icon){
	case 1:
		ADC_ChannelConfig(ADC_CHANNEL_TEMPSENSOR);
		ILI9341_Draw_String(100, 140, WHITE, BLACK, "Channel TEMPSENSOR:", 2);
		break;
	case 2:
		ADC_ChannelConfig(ADC_CHANNEL_VREFINT);
		ILI9341_Draw_String(100, 140, WHITE, BLACK, "Channel VREF:      ", 2);
		break;
	case 3:
		ADC_ChannelConfig(ADC_CHANNEL_0);
		ILI9341_Draw_String(100, 140, WHITE, BLACK, "Channel AIN0:      ", 2);
		break;
	}
	HAL_ADC_Start(&hadc1);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
