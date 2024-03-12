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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Low */
osThreadId_t Task_LowHandle;
const osThreadAttr_t Task_Low_attributes = {
  .name = "Task_Low",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Middle */
osThreadId_t Task_MiddleHandle;
const osThreadAttr_t Task_Middle_attributes = {
  .name = "Task_Middle",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_High */
osThreadId_t Task_HighHandle;
const osThreadAttr_t Task_High_attributes = {
  .name = "Task_High",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Mutex_PI */
osMutexId_t Mutex_PIHandle;
const osMutexAttr_t Mutex_PI_attributes = {
  .name = "Mutex_PI"
};
/* Definitions for BinarySem_PI */
osSemaphoreId_t BinarySem_PIHandle;
const osSemaphoreAttr_t BinarySem_PI_attributes = {
  .name = "BinarySem_PI"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Low(void *argument);
void AppTask_Middle(void *argument);
void AppTask_High(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of Mutex_PI */
  Mutex_PIHandle = osMutexNew(&Mutex_PI_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinarySem_PI */
  BinarySem_PIHandle = osSemaphoreNew(1, 0, &BinarySem_PI_attributes);

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
  /* creation of Task_Low */
  Task_LowHandle = osThreadNew(AppTask_Low, NULL, &Task_Low_attributes);

  /* creation of Task_Middle */
  Task_MiddleHandle = osThreadNew(AppTask_Middle, NULL, &Task_Middle_attributes);

  /* creation of Task_High */
  Task_HighHandle = osThreadNew(AppTask_High, NULL, &Task_High_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Low */
/**
  * @brief  Function implementing the Task_Low thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Low */
void AppTask_Low(void *argument)
{
  /* USER CODE BEGIN AppTask_Low */
  /* Infinite loop */
	uint8_t str1[]="Task_Low take it\r\n";
	uint8_t str2[]="Task_Low give it\r\n";
	uint8_t str3[]="return Task_Low\r\n";
	for(;;)
	{
		//获取信号量
		if(xSemaphoreTake(Mutex_PIHandle, pdMS_TO_TICKS(200))==pdTRUE)  
		{
			printf("%s",str1);
			//模拟任务连续运行
			HAL_Delay(500);		
			printf("%s",str3);
			HAL_Delay(500);
			printf("%s",str2);
			//释放信号量
			xSemaphoreGive(Mutex_PIHandle);		
		}
	}
  /* USER CODE END AppTask_Low */
}

/* USER CODE BEGIN Header_AppTask_Middle */
/**
* @brief Function implementing the Task_Middle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_Middle */
void AppTask_Middle(void *argument)
{
  /* USER CODE BEGIN AppTask_Middle */
  /* Infinite loop */
	uint8_t strMid[]="Task_Middle is running\r\n";
	for(;;)
	{
		printf("%s", strMid);
		vTaskDelay(500);
	}
  /* USER CODE END AppTask_Middle */
}

/* USER CODE BEGIN Header_AppTask_High */
/**
* @brief Function implementing the Task_High thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_High */
void AppTask_High(void *argument)
{
  /* USER CODE BEGIN AppTask_High */
  /* Infinite loop */
	uint8_t strHigh1[]="Into Task_High\r\n";
	uint8_t strHigh2[]="Task_High get token\r\n";
	uint8_t strHigh3[]="Task_High give token\r\n";
	for(;;)
	{
		printf("%s",strHigh1);
		//获取信号量
		if(xSemaphoreTake(Mutex_PIHandle, portMAX_DELAY)==pdTRUE)  
		{
			printf("%s",strHigh2);
			printf("%s",strHigh3);
			//释放信号量
			xSemaphoreGive(Mutex_PIHandle);	
		}
		vTaskDelay(500);
	}
  /* USER CODE END AppTask_High */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

