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
#define MSG_BUFFER_LEN	50		//��Ϣ���������ȣ���λ���ֽ�
#define MSG_MAX_LEN			20		//��Ϣ��󳤶ȣ���λ���ֽ�
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
MessageBufferHandle_t msgBuffer;		//��Ϣ���������
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
	msgBuffer = xMessageBufferCreate(MSG_BUFFER_LEN);   //������Ϣ������
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
	//������������ʱ��������
	uint8_t dtArray[MSG_MAX_LEN];  
	for(;;)
	{
		//��ȡ��Ϣ
		uint16_t realCnt = xMessageBufferReceive(msgBuffer, dtArray, MSG_MAX_LEN, portMAX_DELAY);	
		//ʵ�ʶ����ֽ���
		printf("Read message bytes= %d\r\n", realCnt);
		//��ʾ��������Ϣ�ַ���
		printf("%s\r\n\r\n", dtArray);	
	}
  /* USER CODE END AppTask_Show */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*���ڻ��ѻص�����*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	if (HAL_RTC_GetTime(hrtc, &sTime,  RTC_FORMAT_BIN) != HAL_OK)
		return;
	if (HAL_RTC_GetDate(hrtc, &sDate,  RTC_FORMAT_BIN) !=HAL_OK)
		return;

	//�洢��Ϣ�����飬 MSG_MAX_LEN=20
	char dtArray[MSG_MAX_LEN];   
	//����ż�룬���Ͳ�ͬ���ȵ���Ϣ�ַ���
	if ((sTime.Seconds % 2)==0)  
		 //ת��Ϊ�ַ���,�Զ���'\0'
		sprintf(dtArray,"Seconds = %u",sTime.Seconds); 
	else
		//ת��Ϊ�ַ���,�Զ���'\0'
		sprintf(dtArray,"Minute= %u",sTime.Minutes);  	
	//�ַ������ȣ��������Ľ�����
	uint8_t bytesCount = strlen(dtArray);		

	BaseType_t  highTaskWoken=pdFALSE;
	if(msgBuffer != NULL)
	{
		// bytesCount+1����������'\0'
		uint16_t realCnt = xMessageBufferSendFromISR(msgBuffer, dtArray, bytesCount+1, &highTaskWoken);  

		printf("Write bytes = %d\r\n", realCnt);
		portYIELD_FROM_ISR(highTaskWoken);  //�������һ���������
	}

}
/* USER CODE END Application */

