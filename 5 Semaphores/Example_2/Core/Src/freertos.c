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
/* Definitions for task_KEY2 */
osThreadId_t task_KEY2Handle;
const osThreadAttr_t task_KEY2_attributes = {
  .name = "task_KEY2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CountingSem_Tables */
osSemaphoreId_t CountingSem_TablesHandle;
const osSemaphoreAttr_t CountingSem_Tables_attributes = {
  .name = "CountingSem_Tables"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TASK_KEY2(void *argument);

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
  /* creation of CountingSem_Tables */
  CountingSem_TablesHandle = osSemaphoreNew(5, 5, &CountingSem_Tables_attributes);

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
  /* creation of task_KEY2 */
  task_KEY2Handle = osThreadNew(TASK_KEY2, NULL, &task_KEY2_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TASK_KEY2 */
/**
  * @brief  Function implementing the task_KEY2 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TASK_KEY2 */
void TASK_KEY2(void *argument)
{
  /* USER CODE BEGIN TASK_KEY2 */
  /* Infinite loop */
  for(;;)
  {
		if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
		{
			BaseType_t result = xSemaphoreTake(CountingSem_TablesHandle, pdMS_TO_TICKS(100));
			if(result == pdTRUE) printf("Check In OK\r\n");
			else printf("Check In Fail\r\n");
			//按键消抖
			osDelay(pdMS_TO_TICKS(300));
		}
		else
		{
			UBaseType_t AvailableTables = uxSemaphoreGetCount(CountingSem_TablesHandle);
			printf("Now AvailableTables is : %d\r\n", (uint16_t)AvailableTables);
			osDelay(pdMS_TO_TICKS(10));	
		}
  }
  /* USER CODE END TASK_KEY2 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*周期唤醒回调函数*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    if(CountingSem_TablesHandle != NULL)
		{
			BaseType_t highTaskWoken = pdFALSE;
			//释放计数信号量
			xSemaphoreGiveFromISR(CountingSem_TablesHandle, &highTaskWoken);
			portYIELD_FROM_ISR(highTaskWoken);
		}
}
/* USER CODE END Application */

