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
#include "defines.h"
#include "TaskIO.h"
#include "TaskAppCAN.h"
#include "TaskAppSerial.h"
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
osThreadId TaskIOHandle;
osThreadId TaskAppCANHandle;
osThreadId TaskAppSerialHandle;
osMessageQId QueueIOHandle;
osMessageQId QueueAppCANHandle;
osMessageQId QueueAppSerialHandle;
osTimerId TimerIOHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TaskIO_Init(void const * argument);
void TaskAppCAN_Init(void const * argument);
void TaskAppSerial_Init(void const * argument);
void vTimerCallbackIo(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

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
  /* definition and creation of TimerIO */
  osTimerDef(TimerIO, vTimerCallbackIo);
  TimerIOHandle = osTimerCreate(osTimer(TimerIO), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of QueueIO */
  osMessageQDef(QueueIO, 8, sMessageType);
  QueueIOHandle = osMessageCreate(osMessageQ(QueueIO), NULL);

  /* definition and creation of QueueAppCAN */
  osMessageQDef(QueueAppCAN, 16, sMessageType);
  QueueAppCANHandle = osMessageCreate(osMessageQ(QueueAppCAN), NULL);

  /* definition and creation of QueueAppSerial */
  osMessageQDef(QueueAppSerial, 16, sMessageType);
  QueueAppSerialHandle = osMessageCreate(osMessageQ(QueueAppSerial), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TaskIO */
  osThreadDef(TaskIO, TaskIO_Init, osPriorityIdle, 0, 128);
  TaskIOHandle = osThreadCreate(osThread(TaskIO), NULL);

  /* definition and creation of TaskAppCAN */
  osThreadDef(TaskAppCAN, TaskAppCAN_Init, osPriorityIdle, 0, 128);
  TaskAppCANHandle = osThreadCreate(osThread(TaskAppCAN), NULL);

  /* definition and creation of TaskAppSerial */
  osThreadDef(TaskAppSerial, TaskAppSerial_Init, osPriorityIdle, 0, 128);
  TaskAppSerialHandle = osThreadCreate(osThread(TaskAppSerial), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_TaskIO_Init */
/**
  * @brief  Function implementing the TaskIO thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TaskIO_Init */
void TaskIO_Init(void const * argument)
{
  /* USER CODE BEGIN TaskIO_Init */
  /* Infinite loop */
  TaskIO_Entry(QueueIOHandle,TimerIOHandle);
  for(;;)
  {
    osDelay(50);
    vTaskIO(argument);
  }
  /* USER CODE END TaskIO_Init */
}

/* USER CODE BEGIN Header_TaskAppCAN_Init */
/**
* @brief Function implementing the TaskAppCAN thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskAppCAN_Init */
void TaskAppCAN_Init(void const * argument)
{
  /* USER CODE BEGIN TaskAppCAN_Init */
  /* Infinite loop */
  TaskAppCAN_Entry(QueueAppCANHandle,NULL);
  for(;;)
  {
    osDelay(1);
    vTaskAppCAN(argument);
  }
  /* USER CODE END TaskAppCAN_Init */
}

/* USER CODE BEGIN Header_TaskAppSerial_Init */
/**
* @brief Function implementing the TaskAppSerial thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskAppSerial_Init */
void TaskAppSerial_Init(void const * argument)
{
  /* USER CODE BEGIN TaskAppSerial_Init */
  /* Infinite loop */
  TaskAppSerial_Entry(QueueAppSerialHandle,NULL);
  for(;;)
  {
    osDelay(1);
    vTaskAppSerial(argument);
  }
  /* USER CODE END TaskAppSerial_Init */
}

/* vTimerCallbackIo function */
void vTimerCallbackIo(void const * argument)
{
  /* USER CODE BEGIN vTimerCallbackIo */
	vLocalTimerCallbackIo(argument);
  /* USER CODE END vTimerCallbackIo */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

