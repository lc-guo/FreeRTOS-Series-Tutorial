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
#include "queue.h"
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
/* Definitions for task_SEND */
osThreadId_t task_SENDHandle;
const osThreadAttr_t task_SEND_attributes = {
  .name = "task_SEND",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for task_RECEIVE */
osThreadId_t task_RECEIVEHandle;
const osThreadAttr_t task_RECEIVE_attributes = {
  .name = "task_RECEIVE",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TEST_QUEUE */
osMessageQueueId_t TEST_QUEUEHandle;
const osMessageQueueAttr_t TEST_QUEUE_attributes = {
  .name = "TEST_QUEUE"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TASK_SEND(void *argument);
void TASK_RECEIVE(void *argument);

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

  /* Create the queue(s) */
  /* creation of TEST_QUEUE */
  TEST_QUEUEHandle = osMessageQueueNew (10, sizeof(uint16_t), &TEST_QUEUE_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of task_SEND */
  task_SENDHandle = osThreadNew(TASK_SEND, NULL, &task_SEND_attributes);

  /* creation of task_RECEIVE */
  task_RECEIVEHandle = osThreadNew(TASK_RECEIVE, NULL, &task_RECEIVE_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TASK_SEND */
/**
  * @brief  Function implementing the task_SEND thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TASK_SEND */
void TASK_SEND(void *argument)
{
  /* USER CODE BEGIN TASK_SEND */
	uint16_t key_value = 0;
  /* Infinite loop */
  for(;;)
  {
		key_value = 0;
		
		if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
			key_value = 3;
		
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
			key_value = 2;
		
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin) == GPIO_PIN_RESET)
			key_value = 1;
		
		if(key_value != 0)
		{
			BaseType_t err = xQueueSendToBack(TEST_QUEUEHandle, &key_value, pdMS_TO_TICKS(50));
			
			if(err == errQUEUE_FULL)
			{
				xQueueReset(TEST_QUEUEHandle);
			}
			//按键消抖
			osDelay(300);
		}
		else
			osDelay(10);
  }
  /* USER CODE END TASK_SEND */
}

/* USER CODE BEGIN Header_TASK_RECEIVE */
/**
* @brief Function implementing the task_RECEIVE thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TASK_RECEIVE */
void TASK_RECEIVE(void *argument)
{
  /* USER CODE BEGIN TASK_RECEIVE */
	UBaseType_t msgCount=0,freeSpace=0;
	uint16_t key_value=0;
  /* Infinite loop */
  for(;;)
  {
		msgCount = uxQueueMessagesWaiting(TEST_QUEUEHandle);
		freeSpace = uxQueueSpacesAvailable(TEST_QUEUEHandle);
		BaseType_t result = xQueueReceive(TEST_QUEUEHandle, &key_value, pdMS_TO_TICKS(50));
		
		if(result != pdTRUE)
			continue;
		
		printf("msgCount: %d, freeSpace: %d, key_value: %d\r\n", (uint16_t)msgCount, (uint16_t)freeSpace, key_value);
		
    osDelay(100);
  }
  /* USER CODE END TASK_RECEIVE */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*周期唤醒回调函数*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	uint16_t key_value = 9;
	BaseType_t pxHigherPriorityTaskWoken;
	//向队列中发送数据，中断安全版本
	xQueueSendToBackFromISR(TEST_QUEUEHandle, &key_value, &pxHigherPriorityTaskWoken);
	//进行上下文切换
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}
/* USER CODE END Application */

