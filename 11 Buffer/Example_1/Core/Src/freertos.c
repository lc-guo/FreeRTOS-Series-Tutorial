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
#include "stream_buffer.h"
#include "stdio.h"
#include "adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define  BUFFER_LEN     80		//����������С���ֽ���
#define  TRIGGER_LEVEL  20		//����ˮƽ���ֽ���
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
StreamBufferHandle_t  streamBuf;		//���������������
/* USER CODE END Variables */
/* Definitions for Task_Main */
osThreadId_t Task_MainHandle;
const osThreadAttr_t Task_Main_attributes = {
  .name = "Task_Main",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Main(void *argument);

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
  /* creation of Task_Main */
  Task_MainHandle = osThreadNew(AppTask_Main, NULL, &Task_Main_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	streamBuf=xStreamBufferCreate(BUFFER_LEN, TRIGGER_LEVEL);   //������������
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Main */
/**
  * @brief  Function implementing the Task_Main thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Main */
void AppTask_Main(void *argument)
{
  /* USER CODE BEGIN AppTask_Main */
	//���������������
	printf("Stream Buffer length = %d\r\n", BUFFER_LEN);
	//�������ˮƽ
	printf("Trigger Level = %d\r\n", TRIGGER_LEVEL);
	//ÿ�δ�����������ȡ8���ֽ�����
	uint16_t requiredBytes = 32;  //TRIGGER_LEVEL=20;
	
	printf("Required bytes= %d\r\n", requiredBytes);
	//����ADC���ݻ����������洢10���ɼ�ֵ
	uint32_t adcArray[10];

	for(;;)
	{
		uint16_t actualReadBytes = xStreamBufferReceive(streamBuf, adcArray, requiredBytes, portMAX_DELAY);
		//��ʾʵ�ʶ����ֽ�
		printf("Actual read bytes= %d\r\n", actualReadBytes);	
		//ʵ�ʶ�ȡ���ݵ����, ÿ������4�ֽ�
		uint8_t actualItems = actualReadBytes / 4;
		//���
		uint32_t sum=0;
		for( uint8_t i=0; i<actualItems; i++)
			sum += adcArray[i];
		//����ƽ��ֵ
		sum= sum/actualItems;  
		//���ƽ��ֵ
		printf("Average ADC Value= %d\r\n", sum);
	}

  /* USER CODE END AppTask_Main */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*ת������жϻص�*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance != ADC1)
		return;
	//��ȡADCԭʼ�ɼ�ֵ
	uint32_t adc_value=HAL_ADC_GetValue(hadc); 
	BaseType_t  highTaskWoken=pdFALSE;
	if (streamBuf != NULL)
	{
		//�����ݷ��͵�����������
		xStreamBufferSendFromISR(streamBuf, &adc_value, 4, &highTaskWoken);
		//������������л�
		portYIELD_FROM_ISR(highTaskWoken); 

	}
}
/* USER CODE END Application */

