/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "stdio.h"
#include "Touch.h"
#include "adc.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId_t TouchscreenReadHandle;
osThreadId_t ADC_ReadoutHandle;
osMessageQueueId_t iconQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void vTouchscreenRead(void *argument);
void vADC_Readout(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	;
	HAL_UART_Transmit(&huart2, "Stack overflow detected.", 25 , 100);
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */
osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of iconQueue */
  const osMessageQueueAttr_t iconQueue_attributes = {
    .name = "iconQueue"
  };
  iconQueueHandle = osMessageQueueNew (5, sizeof(uint8_t), &iconQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TouchscreenRead */
  const osThreadAttr_t TouchscreenRead_attributes = {
    .name = "TouchscreenRead",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512
  };
  TouchscreenReadHandle = osThreadNew(vTouchscreenRead, NULL, &TouchscreenRead_attributes);

  /* definition and creation of ADC_Readout */
  const osThreadAttr_t ADC_Readout_attributes = {
    .name = "ADC_Readout",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512
  };
  ADC_ReadoutHandle = osThreadNew(vADC_Readout, NULL, &ADC_Readout_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_vTouchscreenRead */
/**
  * @brief  Function implementing the TouchscreenRead thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_vTouchscreenRead */
void vTouchscreenRead(void *argument)
{
    
    
    

  /* USER CODE BEGIN vTouchscreenRead */
	uint8_t iconPressed = 0;
	uint16_t xtemp, ytemp;
  /* Infinite loop */
  for(;;)
  {
//	  	osMessageQueuePut(iconQueueHandle, &iconPressed, 0U, pdMS_TO_TICKS(100));
	  	//osThreadYield();
		tp_dev.scan(0);
		if (tp_dev.sta & TP_PRES_DOWN) {
			xtemp = TP_Read_XOY(0xD0);
			ytemp = TP_Read_XOY(0x90);

			if (xtemp > 790 && xtemp < 1300 && ytemp > 840 && ytemp < 1400 && iconPressed != 1){   // 1st icon pressed
				iconPressed = 1;
				ADC_ConfigAndRun(iconPressed);
				osMessageQueuePut(iconQueueHandle, &iconPressed, 0U, 0U);
			}
			else if (xtemp > 790 && xtemp < 1300 && ytemp > 1880 && ytemp < 2560 && iconPressed != 2){   // 2nd icon pressed
				iconPressed = 2;
				ADC_ConfigAndRun(iconPressed);
				osMessageQueuePut(iconQueueHandle, &iconPressed, 0U, 0U);
			}
			else if (xtemp > 790 && xtemp < 1300 && ytemp > 2900 && ytemp < 3600 && iconPressed != 3){  // 3rd icon pressed
				iconPressed = 3;
				ADC_ConfigAndRun(iconPressed);
				osMessageQueuePut(iconQueueHandle, &iconPressed, 0U, 0U);
			}

		}
    osDelay(1);
  }
  /* USER CODE END vTouchscreenRead */
}

/* USER CODE BEGIN Header_vADC_Readout */
/**
* @brief Function implementing the ADC_Readout thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vADC_Readout */
void vADC_Readout(void *argument)
{
  /* USER CODE BEGIN vADC_Readout */
	uint8_t iconPressed = 0;
	char display_string[30] = { '0' };
	osStatus_t status;
  /* Infinite loop */
  for(;;)
  {
	status = osMessageQueueGet(iconQueueHandle, &iconPressed, NULL, 0U);
	if (status == osOK) {
		if (iconPressed){
			if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
				uint32_t adc = HAL_ADC_GetValue(&hadc1);
				float voltage = (float) adc * 3.3f / 4096.0f;
				int intVoltage = (int)voltage;
				int decSpaces = (int)((voltage-intVoltage)*1000);
				snprintf(display_string, 30, "Voltage: %d.%d V     ", intVoltage, decSpaces );
				HAL_UART_Transmit(&huart2, (uint8_t*) display_string, strlen(display_string), 0xFFFF);
				//ILI9341_Draw_String(80, 160, WHITE, BLACK, display_string, 2);
				HAL_ADC_Start(&hadc1);
			}
		}
	}
    osDelay(1);
  }
  /* USER CODE END vADC_Readout */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
// Replace HAL timing functions with FreeRTOS functions
// Pero, Feb. 2020
//void HAL_Delay(uint32_t Delay)
//{
//    osDelay(Delay);
//}
//
//uint32_t HAL_GetTick(void)
//{
//    return osKernelGetTickCount();
//}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
