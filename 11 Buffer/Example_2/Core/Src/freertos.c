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
#include "message_buffer.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MSG_BUFFER_LEN	50		//消息缓存区长度，单位：字节
#define MSG_MAX_LEN			20		//消息最大长度，单位：字节
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
MessageBufferHandle_t msgBuffer;		//消息缓存区句柄
/* USER CODE END Variables */
/* Definitions for Task_Show */
osThreadId_t Task_ShowHandle;
const osThreadAttr_t Task_Show_attributes = {
  .name = "Task_Show",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Show(void *argument);

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
  /* creation of Task_Show */
  Task_ShowHandle = osThreadNew(AppTask_Show, NULL, &Task_Show_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	msgBuffer = xMessageBufferCreate(MSG_BUFFER_LEN);   //创建消息缓存区
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Show */
/**
  * @brief  Function implementing the Task_Show thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Show */
void AppTask_Show(void *argument)
{
  /* USER CODE BEGIN AppTask_Show */
  /* Infinite loop */
	//读出的数据临时保存数组
	uint8_t dtArray[MSG_MAX_LEN];  
	for(;;)
	{
		//读取消息
		uint16_t realCnt = xMessageBufferReceive(msgBuffer, dtArray, MSG_MAX_LEN, portMAX_DELAY);	
		//实际读出字节数
		printf("Read message bytes= %d\r\n", realCnt);
		//显示读出的消息字符串
		printf("%s\r\n\r\n", dtArray);	
	}
  /* USER CODE END AppTask_Show */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*周期唤醒回调函数*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	if (HAL_RTC_GetTime(hrtc, &sTime,  RTC_FORMAT_BIN) != HAL_OK)
		return;
	if (HAL_RTC_GetDate(hrtc, &sDate,  RTC_FORMAT_BIN) !=HAL_OK)
		return;

	//存储消息的数组， MSG_MAX_LEN=20
	char dtArray[MSG_MAX_LEN];   
	//分奇偶秒，发送不同长度的消息字符串
	if ((sTime.Seconds % 2)==0)  
		 //转换为字符串,自动加'\0'
		sprintf(dtArray,"Seconds = %u",sTime.Seconds); 
	else
		//转换为字符串,自动加'\0'
		sprintf(dtArray,"Minute= %u",sTime.Minutes);  	
	//字符串长度，不带最后的结束符
	uint8_t bytesCount = strlen(dtArray);		

	BaseType_t  highTaskWoken=pdFALSE;
	if(msgBuffer != NULL)
	{
		// bytesCount+1，带结束符'\0'
		uint16_t realCnt = xMessageBufferSendFromISR(msgBuffer, dtArray, bytesCount+1, &highTaskWoken);  

		printf("Write bytes = %d\r\n", realCnt);
		portYIELD_FROM_ISR(highTaskWoken);  //申请进行一次任务调度
	}

}
/* USER CODE END Application */

