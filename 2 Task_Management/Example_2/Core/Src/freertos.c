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
#include "adc.h"
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
/* Definitions for task_ADC */
osThreadId_t task_ADCHandle;
const osThreadAttr_t task_ADC_attributes = {
  .name = "task_ADC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_KEY_SCAN */
osThreadId_t task_KEY_SCANHandle;
const osThreadAttr_t task_KEY_SCAN_attributes = {
  .name = "task_KEY_SCAN",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TASK_ADC(void *argument);
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
  /* creation of task_ADC */
  task_ADCHandle = osThreadNew(TASK_ADC, NULL, &task_ADC_attributes);

  /* creation of task_KEY_SCAN */
  task_KEY_SCANHandle = osThreadNew(TASK_KEY_SCAN, NULL, &task_KEY_SCAN_attributes);

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
	TickType_t previousWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		//开始临界代码段，不允许任务调度
		taskENTER_CRITICAL();
		HAL_ADC_Start(&hadc1);
		if(HAL_ADC_PollForConversion(&hadc1,200)==HAL_OK)
		{
				uint32_t val=HAL_ADC_GetValue(&hadc1);
				uint32_t Volt=(3300*val)>>12;
				printf("val:%d, Volt:%d\r\n",val,Volt);
		}
		//结束临界代码段，重新允许任务调度
		taskEXIT_CRITICAL();
		vTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(500));
  }
  /* USER CODE END TASK_ADC */
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
	TaskHandle_t taskHandle = task_ADCHandle;
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
				taskHandle = task_ADCHandle;
				TaskStatus_t taskInfo;
				//是否获取高水位值
				BaseType_t getFreeStackSpace = pdTRUE;  	
				//当前的状态，设置为eInvalid将自动获取任务状态
				eTaskState taskState = eInvalid; 		
				//获取任务信息					
				vTaskGetInfo(taskHandle, &taskInfo, getFreeStackSpace, taskState);	
				//开始临界代码段，不允许任务调度	
				taskENTER_CRITICAL();			
				printf("\r\n--- KEY2 PRESSED ---\r\n");
				printf("Task_Info: Show task info，Get by vTaskGetInfo();\r\n");
				printf("Task Name = %s\r\n", (uint8_t *)taskInfo.pcTaskName);
				printf("Task Number = %d\r\n", (uint16_t)taskInfo.xTaskNumber);
				printf("Task State = %d\r\n", taskInfo.eCurrentState);
				printf("Task Priority = %d\r\n", (uint8_t)taskInfo.uxCurrentPriority);
				printf("High Water Mark = %d\r\n\r\n", taskInfo.usStackHighWaterMark);
				//结束临界代码段，重新允许任务调度
				taskEXIT_CRITICAL();		

			}
			else if(key_value == 2)
			{
				//开始临界代码段，不允许任务调度	
				taskENTER_CRITICAL();
				printf("\r\n--- KEY1 PRESSED ---\r\n");
				//获取空闲任务句柄
				taskHandle = xTaskGetIdleTaskHandle();
				//获取任务高水位值				
				UBaseType_t hwm = uxTaskGetStackHighWaterMark(taskHandle);
				printf("Idle Task'Stack High Water Mark = %d\r\n", (uint16_t)hwm);
				//Task_ADC的任务句柄
				taskHandle=task_ADCHandle;				
				hwm = uxTaskGetStackHighWaterMark(taskHandle);
				printf("Task_ADC'Stack High Water Mark = %d\r\n", (uint16_t)hwm);
				//Task_KEY_SCAN的任务句柄
				taskHandle=task_KEY_SCANHandle;				
				hwm = uxTaskGetStackHighWaterMark(taskHandle);
				printf("Task_KEY_SCAN'Stack High Water Mark = %d\r\n", (uint16_t)hwm);
				//获取系统任务个数
				UBaseType_t taskNum=uxTaskGetNumberOfTasks();  
				printf("There are now %d tasks in total！\r\n\r\n", (uint16_t)taskNum);
				//结束临界代码段，重新允许任务调度
				taskEXIT_CRITICAL();
			}
			else if(key_value == 1)
			{
				//开始临界代码段，不允许任务调度	
				taskENTER_CRITICAL();
				printf("\r\n--- KEY0 PRESSED ---\r\n");
				char infoBuffer[300];
				//获取任务列表
				vTaskList(infoBuffer);
				printf("%s\r\n\r\n",infoBuffer);
				//结束临界代码段，重新允许任务调度
				taskEXIT_CRITICAL();
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

