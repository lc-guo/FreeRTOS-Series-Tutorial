/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint32_t adc_value;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for task_ADC */
osThreadId_t task_ADCHandle;
const osThreadAttr_t task_ADC_attributes = {
  .name = "task_ADC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BinarySem_ADC */
osSemaphoreId_t BinarySem_ADCHandle;
const osSemaphoreAttr_t BinarySem_ADC_attributes = {
  .name = "BinarySem_ADC"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TASK_ADC(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinarySem_ADC */
  BinarySem_ADCHandle = osSemaphoreNew(1, 0, &BinarySem_ADC_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of task_ADC */
  task_ADCHandle = osThreadNew(TASK_ADC, NULL, &task_ADC_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TASK_ADC */
/**
  * @brief  Function implementing the task_ADC thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TASK_ADC */
void TASK_ADC(void *argument)
{
  /* USER CODE BEGIN TASK_ADC */
  /* Infinite loop */
  for(;;)
  {
		if(xSemaphoreTake(BinarySem_ADCHandle, portMAX_DELAY) == pdTRUE)
		{
			uint32_t Volt = (3300 * adc_value)>>12;
			printf("val:%d, Volt:%d\r\n", adc_value, Volt);
		}
  }
  /* USER CODE END TASK_ADC */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*转换完成中断回调*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    /*定时器中断启动单通道转换*/
    if(hadc->Instance == ADC1)
    {
        adc_value = HAL_ADC_GetValue(hadc);
				BaseType_t highTaskWoken = pdFALSE;
				if(BinarySem_ADCHandle != NULL)
				{
					xSemaphoreGiveFromISR(BinarySem_ADCHandle, &highTaskWoken);
					portYIELD_FROM_ISR(highTaskWoken);
				}
    }
}
/* USER CODE END Application */

