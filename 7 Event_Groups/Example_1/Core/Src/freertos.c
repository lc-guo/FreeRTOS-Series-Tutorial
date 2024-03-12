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
#define mainFIRST_TASK_BIT 	( 1UL << 0UL ) /* 事件组位 0 */
#define mainSECOND_TASK_BIT ( 1UL << 1UL ) /* 事件组位 1 */
#define mainISR_BIT 				( 1UL << 2UL ) /* 事件组位 2 */
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
	/* 400ms延时变量 */
	const TickType_t xDelay400ms = pdMS_TO_TICKS(400UL);
  /* Infinite loop */
  for(;;)
  {
		/* 在下次循环开始之前短延时 */
		vTaskDelay(xDelay400ms);
		/* 输出事件组位 0 被置位任务置 1 信息 */
		printf("Bit setting task -\t about to set bit 0.\r\n");
		xEventGroupSetBits(xEventGroupHandle, mainFIRST_TASK_BIT);
		/* 在置位下一位之前短延时 */
		vTaskDelay(xDelay400ms);
		/* 输出事件组位 1 被置位任务置 1 信息 */
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
	/* 创建事件组 */
	EventBits_t xEventGroupValue;
	/* 设置要测试的位 */
	const EventBits_t xBitsToWaitFor = (mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainISR_BIT);
  /* Infinite loop */
  for(;;)
  {
    xEventGroupValue = xEventGroupWaitBits( /* 被读的事件组 */
																						xEventGroupHandle,
																						/* 要测试的位 */
																						xBitsToWaitFor,
																						/* 阻塞条件满足退出时清除所有事件位 */
																						pdTRUE,
																						/* 不等待所有位. */
																						pdTRUE,
																						/* 永远等待，不会超时 */
																						portMAX_DELAY );
		/* 位 0 被置 1 */
		if((xEventGroupValue & mainFIRST_TASK_BIT) != 0)
		{
			printf("Bit reading task -\t Event bit 0 was set\r\n");
		}
		/* 位 1 被置 1 */
		if((xEventGroupValue & mainSECOND_TASK_BIT ) != 0 )
		{
			printf("Bit reading task -\t Event bit 1 was set\r\n");
		}
		/* 位 2 被置 1 */
		if((xEventGroupValue & mainISR_BIT ) != 0 )
		{
			printf("Bit reading task -\t Event bit 2 was set\r\n");
		}
  }
  /* USER CODE END AppTask_ReadBits */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*周期唤醒回调函数*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	/* 输出信息提示 */
	printf("Bit setting ISR -\t about to set bit 2.\r\n");
	/* 从中断中设置事件组位 2 为 1 */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xEventGroupSetBitsFromISR(xEventGroupHandle, mainISR_BIT, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* USER CODE END Application */

