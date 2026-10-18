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

#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
typedef enum{
	DISP_NONE = 0,
	DISP_KEY1,
	DISP_KEY2,
} DispCmd_t;

typedef struct{
	DispCmd_t Cmd;
	uint32_t param;
}DispMsg_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin;
  osTimerId timerHandle;
  volatile uint8_t pressed;   // �����¼���־
  uint8_t lastState;          // �ϴβ�����ƽ
  uint8_t stableCnt;          // ����һ�¼���
} KeyObject_t;

KeyObject_t KEY1 = { KEY1_GPIO_Port, KEY1_Pin, NULL, 0, 1, 0 };
KeyObject_t KEY2 = { KEY2_GPIO_Port, KEY2_Pin, NULL, 0, 1, 0 };
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
/* Definitions for KEY1Timer */
osTimerId_t KEY1TimerHandle;
const osTimerAttr_t KEY1Timer_attributes = {
  .name = "KEY1Timer"
};
/* Definitions for KEY2Timer */
osTimerId_t KEY2TimerHandle;
const osTimerAttr_t KEY2Timer_attributes = {
  .name = "KEY2Timer"
};

extern const uint8_t Img[][72];

osMessageQueueId_t dispQueue;
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void PWM_Task_function(void *argument);
void KEY_Task_Function(void *argument);
void OLED_Task_Function(void *argument);
void KEY1Callback(void *argument);
void KEY2Callback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	dispQueue = osMessageQueueNew(5, sizeof(DispMsg_t), NULL);
	if(dispQueue == NULL){
		Error_Handler();
	}
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of KEY1Timer */
  KEY1TimerHandle = osTimerNew(KEY1Callback, osTimerPeriodic, NULL, &KEY1Timer_attributes);

  /* creation of KEY2Timer */
  KEY2TimerHandle = osTimerNew(KEY2Callback, osTimerPeriodic, NULL, &KEY2Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
	KEY1.timerHandle = KEY1TimerHandle;
	KEY2.timerHandle = KEY2TimerHandle;
	
	osTimerStart(KEY1TimerHandle, 10);
	osTimerStart(KEY2TimerHandle, 10);
  /* USER CODE END RTOS_TIMERS */

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
	MG310_SetSpeed_Percent(1000);
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

  for(;;)
  {
		if (KEY1.pressed)
    {
      KEY1.pressed = 0;
      // ����1ҵ���߼�
      HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_SET);
    }
    if (KEY2.pressed)
    {
      KEY2.pressed = 0;
      // ����2ҵ���߼�
      HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_RESET);
    }
    osDelay(10);
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

/* KEY1Callback function */
void KEY1Callback(void *argument)
{
  /* USER CODE BEGIN KEY1Callback */
	uint8_t cur = HAL_GPIO_ReadPin(KEY1.port, KEY1.pin);
  if (cur == KEY1.lastState)
  {
    if (KEY1.stableCnt < 3) KEY1.stableCnt++;
    if (KEY1.stableCnt == 3 && cur == GPIO_PIN_RESET)
    {
      KEY1.pressed = 1;
      KEY1.stableCnt = 4;  // ��ֹ�ظ�����
    }
  }
  else
  {
    KEY1.stableCnt = 0;
    KEY1.lastState = cur;
  }
  /* USER CODE END KEY1Callback */
}

/* KEY2Callback function */
void KEY2Callback(void *argument)
{
  /* USER CODE BEGIN KEY2Callback */
	uint8_t cur = HAL_GPIO_ReadPin(KEY2.port, KEY2.pin);
  if (cur == KEY2.lastState)
  {
    if (KEY2.stableCnt < 3) KEY2.stableCnt++;
    if (KEY2.stableCnt == 3 && cur == GPIO_PIN_RESET)
    {
      KEY2.pressed = 1;
      KEY2.stableCnt = 4;  // ��ֹ�ظ�����
    }
  }
  else
  {
    KEY2.stableCnt = 0;
    KEY2.lastState = cur;
  }
  /* USER CODE END KEY2Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

