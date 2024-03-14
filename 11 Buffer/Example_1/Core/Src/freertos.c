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
#define  BUFFER_LEN     80		//流缓存区大小，字节数
#define  TRIGGER_LEVEL  20		//触发水平，字节数
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
StreamBufferHandle_t  streamBuf;		//流缓冲区句柄变量
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
	streamBuf=xStreamBufferCreate(BUFFER_LEN, TRIGGER_LEVEL);   //创建流缓存区
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
	//输出流缓冲区长度
	printf("Stream Buffer length = %d\r\n", BUFFER_LEN);
	//输出触发水平
	printf("Trigger Level = %d\r\n", TRIGGER_LEVEL);
	//每次从流缓冲区获取8个字节数据
	uint16_t requiredBytes = 32;  //TRIGGER_LEVEL=20;
	
	printf("Required bytes= %d\r\n", requiredBytes);
	//接收ADC数据缓存区，最多存储10个采集值
	uint32_t adcArray[10];

	for(;;)
	{
		uint16_t actualReadBytes = xStreamBufferReceive(streamBuf, adcArray, requiredBytes, portMAX_DELAY);
		//显示实际读出字节
		printf("Actual read bytes= %d\r\n", actualReadBytes);	
		//实际读取数据点个数, 每个数据4字节
		uint8_t actualItems = actualReadBytes / 4;
		//求和
		uint32_t sum=0;
		for( uint8_t i=0; i<actualItems; i++)
			sum += adcArray[i];
		//计算平均值
		sum= sum/actualItems;  
		//输出平均值
		printf("Average ADC Value= %d\r\n", sum);
	}

  /* USER CODE END AppTask_Main */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*转换完成中断回调*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance != ADC1)
		return;
	//获取ADC原始采集值
	uint32_t adc_value=HAL_ADC_GetValue(hadc); 
	BaseType_t  highTaskWoken=pdFALSE;
	if (streamBuf != NULL)
	{
		//将数据发送到流缓冲区中
		xStreamBufferSendFromISR(streamBuf, &adc_value, 4, &highTaskWoken);
		//申请进行任务切换
		portYIELD_FROM_ISR(highTaskWoken); 

	}
}
/* USER CODE END Application */

