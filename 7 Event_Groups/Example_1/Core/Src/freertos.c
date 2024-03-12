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
#include "event_groups.h"
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
#define mainFIRST_TASK_BIT 	( 1UL << 0UL ) /* �¼���λ 0 */
#define mainSECOND_TASK_BIT ( 1UL << 1UL ) /* �¼���λ 1 */
#define mainISR_BIT 				( 1UL << 2UL ) /* �¼���λ 2 */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_SetBits */
osThreadId_t Task_SetBitsHandle;
const osThreadAttr_t Task_SetBits_attributes = {
  .name = "Task_SetBits",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_ReadBits */
osThreadId_t Task_ReadBitsHandle;
const osThreadAttr_t Task_ReadBits_attributes = {
  .name = "Task_ReadBits",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for xEventGroup */
osEventFlagsId_t xEventGroupHandle;
const osEventFlagsAttr_t xEventGroup_attributes = {
  .name = "xEventGroup"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_SetBits(void *argument);
void AppTask_ReadBits(void *argument);

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
  /* creation of Task_SetBits */
  Task_SetBitsHandle = osThreadNew(AppTask_SetBits, NULL, &Task_SetBits_attributes);

  /* creation of Task_ReadBits */
  Task_ReadBitsHandle = osThreadNew(AppTask_ReadBits, NULL, &Task_ReadBits_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of xEventGroup */
  xEventGroupHandle = osEventFlagsNew(&xEventGroup_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_SetBits */
/**
  * @brief  Function implementing the Task_SetBits thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_SetBits */
void AppTask_SetBits(void *argument)
{
  /* USER CODE BEGIN AppTask_SetBits */
	/* 400ms��ʱ���� */
	const TickType_t xDelay400ms = pdMS_TO_TICKS(400UL);
  /* Infinite loop */
  for(;;)
  {
		/* ���´�ѭ����ʼ֮ǰ����ʱ */
		vTaskDelay(xDelay400ms);
		/* ����¼���λ 0 ����λ������ 1 ��Ϣ */
		printf("Bit setting task -\t about to set bit 0.\r\n");
		xEventGroupSetBits(xEventGroupHandle, mainFIRST_TASK_BIT);
		/* ����λ��һλ֮ǰ����ʱ */
		vTaskDelay(xDelay400ms);
		/* ����¼���λ 1 ����λ������ 1 ��Ϣ */
		printf("Bit setting task -\t about to set bit 1.\r\n");
		xEventGroupSetBits(xEventGroupHandle, mainSECOND_TASK_BIT);
  }
  /* USER CODE END AppTask_SetBits */
}

/* USER CODE BEGIN Header_AppTask_ReadBits */
/**
* @brief Function implementing the Task_ReadBits thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_ReadBits */
void AppTask_ReadBits(void *argument)
{
  /* USER CODE BEGIN AppTask_ReadBits */
	/* �����¼��� */
	EventBits_t xEventGroupValue;
	/* ����Ҫ���Ե�λ */
	const EventBits_t xBitsToWaitFor = (mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainISR_BIT);
  /* Infinite loop */
  for(;;)
  {
    xEventGroupValue = xEventGroupWaitBits( /* �������¼��� */
																						xEventGroupHandle,
																						/* Ҫ���Ե�λ */
																						xBitsToWaitFor,
																						/* �������������˳�ʱ��������¼�λ */
																						pdTRUE,
																						/* ���ȴ�����λ. */
																						pdTRUE,
																						/* ��Զ�ȴ������ᳬʱ */
																						portMAX_DELAY );
		/* λ 0 ���� 1 */
		if((xEventGroupValue & mainFIRST_TASK_BIT) != 0)
		{
			printf("Bit reading task -\t Event bit 0 was set\r\n");
		}
		/* λ 1 ���� 1 */
		if((xEventGroupValue & mainSECOND_TASK_BIT ) != 0 )
		{
			printf("Bit reading task -\t Event bit 1 was set\r\n");
		}
		/* λ 2 ���� 1 */
		if((xEventGroupValue & mainISR_BIT ) != 0 )
		{
			printf("Bit reading task -\t Event bit 2 was set\r\n");
		}
  }
  /* USER CODE END AppTask_ReadBits */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*���ڻ��ѻص�����*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	/* �����Ϣ��ʾ */
	printf("Bit setting ISR -\t about to set bit 2.\r\n");
	/* ���ж��������¼���λ 2 Ϊ 1 */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xEventGroupSetBitsFromISR(xEventGroupHandle, mainISR_BIT, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* USER CODE END Application */

