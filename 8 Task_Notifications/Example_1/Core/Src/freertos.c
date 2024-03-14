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
	//����һ���������ڱ�ʾ�����������¼�����
	uint32_t ulEventsToProcess;
  /* Infinite loop */
  for(;;)
  {
		//�ȴ�����֪ͨ
		ulEventsToProcess = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
		//����ȵ��¼�
		if(ulEventsToProcess != 0)
		{
			//����������¼���Ϊ0��һֱ��������һ�δ�������¼�����1
			while(ulEventsToProcess > 0)
			{
				uint32_t Volt = (3300 * adc_value)>>12;
				printf("val:%d, Volt:%d\r\n", adc_value, Volt);
				ulEventsToProcess --;
			}
		}
  }
  /* USER CODE END TASK_ADC */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*ת������жϻص�*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	/*��ʱ���ж�������ͨ��ת��*/
	if(hadc->Instance == ADC1)
	{
		//��ȡԭʼADC�ɼ�ֵ
		adc_value = HAL_ADC_GetValue(hadc);
		BaseType_t highTaskWoken = pdFALSE;
		if(task_ADCHandle != NULL)
		{
			//�ɼ���ϸ�TASK_ADC�����Ͳɼ���ϵ�֪ͨ
			vTaskNotifyGiveFromISR(task_ADCHandle, &highTaskWoken);
			portYIELD_FROM_ISR(highTaskWoken);
		}
	}
}
/* USER CODE END Application */

