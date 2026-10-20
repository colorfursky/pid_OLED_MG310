/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tim.h"
#include "OLED.h"
#include "MG310_Driver.h"

#include "semphr.h"

#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
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
/* Definitions for PWM_Task */
osThreadId_t PWM_TaskHandle;
uint32_t PWM_TaskBuffer[ 128 ];
osStaticThreadDef_t PWM_TaskControlBlock;
const osThreadAttr_t PWM_Task_attributes = {
  .name = "PWM_Task",
  .cb_mem = &PWM_TaskControlBlock,
  .cb_size = sizeof(PWM_TaskControlBlock),
  .stack_mem = &PWM_TaskBuffer[0],
  .stack_size = sizeof(PWM_TaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for KEY_Task */
osThreadId_t KEY_TaskHandle;
uint32_t KEY_TaskBuffer[ 128 ];
osStaticThreadDef_t KEY_TaskControlBlock;
const osThreadAttr_t KEY_Task_attributes = {
  .name = "KEY_Task",
  .cb_mem = &KEY_TaskControlBlock,
  .cb_size = sizeof(KEY_TaskControlBlock),
  .stack_mem = &KEY_TaskBuffer[0],
  .stack_size = sizeof(KEY_TaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OLED_Task */
osThreadId_t OLED_TaskHandle;
uint32_t OLED_TaskBuffer[ 128 ];
osStaticThreadDef_t OLED_TaskControlBlock;
const osThreadAttr_t OLED_Task_attributes = {
  .name = "OLED_Task",
  .cb_mem = &OLED_TaskControlBlock,
  .cb_size = sizeof(OLED_TaskControlBlock),
  .stack_mem = &OLED_TaskBuffer[0],
  .stack_size = sizeof(OLED_TaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for KEY_MsgQueue */
osMessageQueueId_t KEY_MsgQueueHandle;
uint8_t KEY_MsgQueueBuffer[ 8 * sizeof( uint8_t ) ];
osStaticMessageQDef_t KEY_MsgQueueControlBlock;
const osMessageQueueAttr_t KEY_MsgQueue_attributes = {
  .name = "KEY_MsgQueue",
  .cb_mem = &KEY_MsgQueueControlBlock,
  .cb_size = sizeof(KEY_MsgQueueControlBlock),
  .mq_mem = &KEY_MsgQueueBuffer,
  .mq_size = sizeof(KEY_MsgQueueBuffer)
};
/* Definitions for KEY1_Semaphore */
osSemaphoreId_t KEY1_SemaphoreHandle;
osStaticSemaphoreDef_t KEY1_SemaphoreControlBlock;
const osSemaphoreAttr_t KEY1_Semaphore_attributes = {
  .name = "KEY1_Semaphore",
  .cb_mem = &KEY1_SemaphoreControlBlock,
  .cb_size = sizeof(KEY1_SemaphoreControlBlock),
};
/* Definitions for KEY2_Semaphore */
osSemaphoreId_t KEY2_SemaphoreHandle;
osStaticSemaphoreDef_t KEY2_SemaphoreControlBlock;
const osSemaphoreAttr_t KEY2_Semaphore_attributes = {
  .name = "KEY2_Semaphore",
  .cb_mem = &KEY2_SemaphoreControlBlock,
  .cb_size = sizeof(KEY2_SemaphoreControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void PWM_Task_function(void *argument);
void KEY_Task_Function(void *argument);
void OLED_Task_Function(void *argument);

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
  /* creation of KEY1_Semaphore */
  KEY1_SemaphoreHandle = osSemaphoreNew(1, 1, &KEY1_Semaphore_attributes);

  /* creation of KEY2_Semaphore */
  KEY2_SemaphoreHandle = osSemaphoreNew(1, 1, &KEY2_Semaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of KEY_MsgQueue */
  KEY_MsgQueueHandle = osMessageQueueNew (8, sizeof(uint8_t), &KEY_MsgQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of PWM_Task */
  PWM_TaskHandle = osThreadNew(PWM_Task_function, NULL, &PWM_Task_attributes);

  /* creation of KEY_Task */
  KEY_TaskHandle = osThreadNew(KEY_Task_Function, NULL, &KEY_Task_attributes);

  /* creation of OLED_Task */
  OLED_TaskHandle = osThreadNew(OLED_Task_Function, NULL, &OLED_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_PWM_Task_function */
/**
  * @brief  Function implementing the PWM_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_PWM_Task_function */
void PWM_Task_function(void *argument)
{
  /* USER CODE BEGIN PWM_Task_function */
  /* Infinite loop */
	// PWM_Init
	MG310_Rotation(ROTATE_FORWARD);
	MG310_Cmd(MG310_ON);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	MG310_SetSpeed_Percent(500);
	// value Init
//	uint8_t occupy = 0;
//	int8_t direction = 1;
  for(;;)
  {
//		occupy += direction;
//		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, occupy * 10);
//		if(occupy == 100)
//			direction = -1;
//		else if(occupy == 0)
//			direction = 1;
    osDelay(10);
  }
  /* USER CODE END PWM_Task_function */
}

/* USER CODE BEGIN Header_KEY_Task_Function */
/**
* @brief Function implementing the KEY_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KEY_Task_Function */
void KEY_Task_Function(void *argument)
{
  /* USER CODE BEGIN KEY_Task_Function */
  /* Infinite loop */
	const TickType_t xTicksToWait = pdMS_TO_TICKS(10);
  for(;;)
  {
		if(xSemaphoreTake(KEY1_SemaphoreHandle, xTicksToWait) == pdTRUE){
			osDelay(20);
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET){
				HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_SET);
				while(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET){osDelay(10);}
			}
		}else if(xSemaphoreTake(KEY2_SemaphoreHandle, xTicksToWait) == pdTRUE){
			osDelay(20);
			if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET){
				HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_RESET);
				while(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET){osDelay(10);}
			}
		}
  }
  /* USER CODE END KEY_Task_Function */
}

/* USER CODE BEGIN Header_OLED_Task_Function */
/**
* @brief Function implementing the OLED_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OLED_Task_Function */
void OLED_Task_Function(void *argument)
{
  /* USER CODE BEGIN OLED_Task_Function */
  /* Infinite loop */
	vTaskDelay(20);
	HAL_GPIO_WritePin(OLED_Pin_GPIO_Port, OLED_Pin_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RES_GPIO_Port, RES_Pin, GPIO_PIN_SET);
	vTaskDelay(100);
	OLED_Init();
	vTaskDelay(100);
	
	float cur_speed;
	
	char strings[15];
	
	OLED_Clear();
	
	OLED_ShowChineseChars(0, 0, "速度");
	
	OLED_Ciallo();

	for(;;)
  {
		cur_speed = MG310_GetCurSpeed();
		sprintf(strings, "%05.2fkm/h", cur_speed);
		OLED_ShowString(24*2, 0, strings, 8);
    osDelay(MEASURE_DELAY);
  }
  /* USER CODE END OLED_Task_Function */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

