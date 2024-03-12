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
#include "stdlib.h"
#include "event_groups.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define mainFIRST_TASK_BIT 	( 1UL << 0UL ) /* 事件组位 0 */
#define mainSECOND_TASK_BIT ( 1UL << 1UL ) /* 事件组位 1 */
#define mainTHIRD_TASK_BIT  ( 1UL << 2UL ) /* 事件组位 2 */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Syncing */
osThreadId_t Task_SyncingHandle;
const osThreadAttr_t Task_Syncing_attributes = {
  .name = "Task_Syncing",
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

void AppTask_Syncing(void *argument);

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
  /* creation of Task_Syncing */
  //Task_SyncingHandle = osThreadNew(AppTask_Syncing, NULL, &Task_Syncing_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	xTaskCreate(AppTask_Syncing, "Task 1", 1000, (void*)mainFIRST_TASK_BIT, osPriorityNormal, NULL );
	xTaskCreate(AppTask_Syncing, "Task 2", 1000, (void*)mainSECOND_TASK_BIT, osPriorityNormal, NULL );
	xTaskCreate(AppTask_Syncing, "Task 3", 1000, (void*)mainTHIRD_TASK_BIT, osPriorityNormal, NULL );
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of xEventGroup */
  xEventGroupHandle = osEventFlagsNew(&xEventGroup_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Syncing */
/**
  * @brief  Function implementing the Task_Syncing thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Syncing */
void AppTask_Syncing(void *argument)
{
  /* USER CODE BEGIN AppTask_Syncing */
	/* 创建两个延时用于合成随机延时时间 */
	const TickType_t xMaxDelay = pdMS_TO_TICKS(4000UL);
	const TickType_t xMinDelay = pdMS_TO_TICKS(200UL);
	/* 延时时间 */
	TickType_t xDelayTime;
	/* 任务要设置的事件组的位 */
	EventBits_t uxThisTasksSyncBit;
	/* 任务要等待的事件组的所有位 */
	const EventBits_t uxAllSyncBits = ( mainFIRST_TASK_BIT |
																			mainSECOND_TASK_BIT |
																			mainTHIRD_TASK_BIT );
	
	uxThisTasksSyncBit = ( EventBits_t )argument;
  /* Infinite loop */
  for(;;)
  {
		/* 合成随机延时时间，模拟三个任务不同时间到达同步点 */
		xDelayTime = (rand() % xMaxDelay) + xMinDelay;
		vTaskDelay(xDelayTime);
		printf("%s reached sync point\r\n", pcTaskGetTaskName(NULL));
		xEventGroupSync(/* 被读的事件组 */
										xEventGroupHandle,
										/* 测试的位 */
										uxThisTasksSyncBit,
										/* 需要等待的所有位 */
										uxAllSyncBits,
										/* 永远等待，不会超时 */
										portMAX_DELAY);
		/* 任务会同时退出同步点，串口输出需要时间，所以通过临界段保护串口输出 */
		taskENTER_CRITICAL();
		printf("%s exited sync point\r\n", pcTaskGetTaskName(NULL));
		taskEXIT_CRITICAL();
  }
  /* USER CODE END AppTask_Syncing */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

