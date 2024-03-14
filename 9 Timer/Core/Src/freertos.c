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
#include "timers.h"
#include "stdio.h"
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
/* Definitions for Task_KeyScan */
osThreadId_t Task_KeyScanHandle;
const osThreadAttr_t Task_KeyScan_attributes = {
  .name = "Task_KeyScan",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TimerPeriodic */
osTimerId_t TimerPeriodicHandle;
const osTimerAttr_t TimerPeriodic_attributes = {
  .name = "TimerPeriodic"
};
/* Definitions for TimerOnce */
osTimerId_t TimerOnceHandle;
const osTimerAttr_t TimerOnce_attributes = {
  .name = "TimerOnce"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_KeyScan(void *argument);
void appTimerPeriodic(void *argument);
void appTimerOnce(void *argument);

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

  /* Create the timer(s) */
  /* creation of TimerPeriodic */
  TimerPeriodicHandle = osTimerNew(appTimerPeriodic, osTimerPeriodic, NULL, &TimerPeriodic_attributes);

  /* creation of TimerOnce */
  TimerOnceHandle = osTimerNew(appTimerOnce, osTimerOnce, NULL, &TimerOnce_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_KeyScan */
  Task_KeyScanHandle = osThreadNew(AppTask_KeyScan, NULL, &Task_KeyScan_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_KeyScan */
/**
  * @brief  Function implementing the Task_KeyScan thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_KeyScan */
void AppTask_KeyScan(void *argument)
{
  /* USER CODE BEGIN AppTask_KeyScan */
	uint8_t key_value = 0;
  /* Infinite loop */
  for(;;)
  {
		key_value = 0;
		//按键WK_UP按下
		if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin) == GPIO_PIN_SET)
			key_value = 4;
		//按键KEY2按下
		if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin) == GPIO_PIN_RESET)
			key_value = 3;
		//按键KEY1按下
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
			key_value = 2;
		//按键KEY0按下
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin) == GPIO_PIN_RESET)
			key_value = 1;
		
		if(key_value != 0)
		{
			if(key_value == 4)
			{
				if(xTimerChangePeriod(TimerPeriodicHandle, 500, pdMS_TO_TICKS(500)) == pdTRUE)
				{
					printf("\r\nWK_UP PRESSED, TimerPeriodic Start!\r\n\r\n");
				}
			}
			if(key_value == 3)
			{
				if(xTimerChangePeriod(TimerOnceHandle, 1000, pdMS_TO_TICKS(500)) == pdTRUE)
				{
					printf("\r\nKEY2 PRESSED, TimerOnce Start!\r\n\r\n");
				}
			}
			else if(key_value == 2)
			{
				if(xTimerReset(TimerPeriodicHandle, pdMS_TO_TICKS(500)) == pdTRUE)
				{
					printf("\r\nKEY1 PRESSED, TimerPeriodic Reset!\r\n\r\n");
				}
			}
			else if(key_value == 1)
			{
				if(xTimerStop(TimerPeriodicHandle, pdMS_TO_TICKS(500)) == pdTRUE)
				{
					printf("\r\nKEY0 PRESSED, TimerPeriod Stop!\r\n\r\n");
				}
			}
			//有按键按下就进行按键消抖
			osDelay(300);
		}
		else
			osDelay(10);
  }
  /* USER CODE END AppTask_KeyScan */
}

/* appTimerPeriodic function */
void appTimerPeriodic(void *argument)
{
  /* USER CODE BEGIN appTimerPeriodic */
	printf("Into appTimerPeriodic Function\r\n");
  /* USER CODE END appTimerPeriodic */
}

/* appTimerOnce function */
void appTimerOnce(void *argument)
{
  /* USER CODE BEGIN appTimerOnce */
	printf("Into appTimerOnce Function\r\n");
  /* USER CODE END appTimerOnce */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

