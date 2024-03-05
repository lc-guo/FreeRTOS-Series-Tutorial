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
typedef StaticTask_t osStaticThreadDef_t;
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
/* Definitions for task_GREEN_LED */
osThreadId_t task_GREEN_LEDHandle;
const osThreadAttr_t task_GREEN_LED_attributes = {
  .name = "task_GREEN_LED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_RED_LED */
osThreadId_t task_RED_LEDHandle;
uint32_t task_RED_LEDBuffer[ 128 ];
osStaticThreadDef_t task_RED_LEDControlBlock;
const osThreadAttr_t task_RED_LED_attributes = {
  .name = "task_RED_LED",
  .cb_mem = &task_RED_LEDControlBlock,
  .cb_size = sizeof(task_RED_LEDControlBlock),
  .stack_mem = &task_RED_LEDBuffer[0],
  .stack_size = sizeof(task_RED_LEDBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_KEY_SCAN */
osThreadId_t task_KEY_SCANHandle;
const osThreadAttr_t task_KEY_SCAN_attributes = {
  .name = "task_KEY_SCAN",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TASK_GREEN_LED(void *argument);
void TASK_RED_LED(void *argument);
void TASK_KEY_SCAN(void *argument);

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
  /* creation of task_GREEN_LED */
  task_GREEN_LEDHandle = osThreadNew(TASK_GREEN_LED, NULL, &task_GREEN_LED_attributes);

  /* creation of task_RED_LED */
  task_RED_LEDHandle = osThreadNew(TASK_RED_LED, NULL, &task_RED_LED_attributes);

  /* creation of task_KEY_SCAN */
  task_KEY_SCANHandle = osThreadNew(TASK_KEY_SCAN, NULL, &task_KEY_SCAN_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TASK_GREEN_LED */
/**
  * @brief  Function implementing the task_GREEN_LED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TASK_GREEN_LED */
void TASK_GREEN_LED(void *argument)
{
  /* USER CODE BEGIN TASK_GREEN_LED */
	TickType_t previousWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		taskENTER_CRITICAL();
		HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port,GREEN_LED_Pin);
		printf("TASK_GREEN_LED, GREEN LED BLINK!\r\n");
		taskEXIT_CRITICAL();
		//周期任务测试，也可使用 osDelayUntil(pdMS_TO_TICKS(100));
		vTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(100));
  }
  /* USER CODE END TASK_GREEN_LED */
}

/* USER CODE BEGIN Header_TASK_RED_LED */
/**
* @brief Function implementing the task_RED_LED thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TASK_RED_LED */
void TASK_RED_LED(void *argument)
{
  /* USER CODE BEGIN TASK_RED_LED */
	TickType_t previousWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		taskENTER_CRITICAL();
		HAL_GPIO_TogglePin(RED_LED_GPIO_Port,RED_LED_Pin);
		printf("TASK_RED_LED, RED LED BLINK!\r\n");
		taskEXIT_CRITICAL();
		//周期任务测试，也可使用 osDelayUntil(pdMS_TO_TICKS(500));
		vTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(500));
		
  }
  /* USER CODE END TASK_RED_LED */
}

/* USER CODE BEGIN Header_TASK_KEY_SCAN */
/**
* @brief Function implementing the task_KEY_SCAN thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TASK_KEY_SCAN */
void TASK_KEY_SCAN(void *argument)
{
  /* USER CODE BEGIN TASK_KEY_SCAN */
	uint8_t key_value = 0;
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
			if(key_value == 3)
			{
				printf("\r\n\r\nKEY2 PRESSED, Delete TASK_GREEN_LED!\r\n\r\n");
				//此处也可使用vTaskDelete(task_GREEN_LEDHandle)，但要注意不能重复删除句柄
				osThreadTerminate(task_GREEN_LEDHandle);
			}
			else if(key_value == 2)
			{
				printf("\r\n\r\nKEY1 PRESSED, Suspend TASK_RED_LED!\r\n\r\n");
				vTaskSuspend(task_RED_LEDHandle);
			}
			else if(key_value == 1)
			{
				printf("\r\n\r\nKEY0 PRESSED, Resume TASK_RED_LED!\r\n\r\n");
				vTaskResume(task_RED_LEDHandle);
			}
			//按键消抖
			osDelay(300);
		}
		else
			osDelay(10);
  }
  /* USER CODE END TASK_KEY_SCAN */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

