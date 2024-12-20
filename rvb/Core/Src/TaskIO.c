/*
 * TaskIO.c
 *
 *  Created on: Nov 1, 2024
 *      Author: volkswagen2
 */
#include "TaskIO.h"
extern void vTimerCallbackIo(void const * argument);
sMessageType stIOMsg;
static unsigned char ucCurrentStateIO = TASKIO_IO_INITIALIZING;
static QueueHandle_t xQueueIO;
static TimerHandle_t xTimerIO;


static uint32_t ulCountPulse;
static uint32_t ulCountTimePulseOn;
static uint32_t ulCountTimePulseOff;
static uint32_t ulCountPeriod;

static uint32_t ulQtyPulse;
static uint32_t ulTimePulseOn;
static uint32_t ulTimePulseOff;
static uint32_t ulPeriod;

#if 0
const long onDuration = 100; // Duration for LED on (ms)
const long offDuration = 300; // Duration for LED off (ms)
const long silentDuration = 1000; // Duration for LED silent period (ms)
long lastStateChangeTime = 0;
unsigned char ledState = false;
long currentTime = 0;
#endif

static QueueHandle_t *pstQueueAppCAN;

//////////////////////////////////////////////
//
//
//              TaskIO_Entry
//
//
//////////////////////////////////////////////
void TaskIO_Entry(QueueHandle_t *xQueue,TimerHandle_t xTimer)
{
    xQueueIO = xQueue;
    xTimerIO = xTimer;

    stIOMsg.ucSrc = SRC_IO;
    stIOMsg.ucDest = SRC_IO;
    stIOMsg.ucEvent = EVENT_IO_LED_INIT;
    xQueueGenericSend(xQueueIO, ( void * )&stIOMsg, 0,0);
}
//////////////////////////////////////////////
//
//
//              TaskIO_Start
//
//
//////////////////////////////////////////////
unsigned char TaskIO_Start(sMessageType *psMessage)
{
    unsigned char boError = true;

    pstQueueAppCAN  = TaskAppCAN_getQueue();

    stIOMsg.ucSrc = SRC_IO;
    stIOMsg.ucDest = SRC_IO;
    stIOMsg.ucEvent = EVENT_IO_LED_NULL;
    xQueueGenericSend(xQueueIO, ( void * )&stIOMsg, 0,0);

    ulCountPulse = 0;
    ulCountTimePulseOn = 0;
    ulCountTimePulseOff = 0;
    ulCountPeriod = 0;

	ulQtyPulse = 5;	/* 5 pulses*/
	ulTimePulseOn = 2; 	/* 2*50 = 100ms*/
	ulTimePulseOff = 2; 	/* 2*50 = 100ms*/
	ulPeriod = 40; 		/* 20*50 = 1s*/

    (void)osTimerStart(xTimerIO,50);
	return boError;
}

//////////////////////////////////////////////
//
//
//              TaskIO_getQueue
//
//
//////////////////////////////////////////////
QueueHandle_t *TaskIO_getQueue()
{
	return &xQueueIO;
}

//////////////////////////////////////////////
//
//
//              TaskIO_PSEvent
//
//
//////////////////////////////////////////////
unsigned char TaskIO_PSEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

    (void)osTimerStop(xTimerIO);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	stIOMsg.ucSrc = SRC_IO;
	stIOMsg.ucDest = SRC_APPCAN;
	stIOMsg.ucEvent = EVENT_APPCAN_SLEEP;
	stIOMsg.pcMessageData = NULL;
    xQueueGenericSend(*pstQueueAppCAN, &stIOMsg, 0,0);

    return(boError);
}

//////////////////////////////////////////////
//
//
//              TaskIO_WakeupEvent
//
//
//////////////////////////////////////////////
unsigned char TaskIO_WakeupEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

    (void)osTimerStart(xTimerIO,50);

    return(boError);
}

//////////////////////////////////////////////
//
//
//              TaskIO_IgnoreEvent
//
//
//////////////////////////////////////////////
unsigned char TaskIO_IgnoreEvent(sMessageType *psMessage)
{
    unsigned char boError = false;

    return(boError);
}

//////////////////////////////////////////////
//
//
//              vTimerCallbackIo
//
//
//////////////////////////////////////////////
void vLocalTimerCallbackIo(void const * argument)
 {
	ulCountPeriod++;
    if(ulCountPeriod <= ulPeriod)
    {
    	if(ulCountPulse <= ((ulQtyPulse) * (ulTimePulseOn+ulTimePulseOff)))
    	{
			if(ulCountTimePulseOn < ulTimePulseOn)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
				ulCountTimePulseOn++;
			}
			else
			{
				if(ulCountTimePulseOff < ulTimePulseOff )
				{
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
					ulCountTimePulseOff++;
				}
				else
				{
					ulCountTimePulseOn = 0;
					ulCountTimePulseOff= 0;
				}
			}
			ulCountPulse++;
    	}
    }
    else
    {
        ulCountPulse = 0;
        ulCountTimePulseOn = 0;
        ulCountTimePulseOff = 0;
        ulCountPeriod = 0;

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		/*xTimerStop(xTimerIO,0);*/
    }
#if 0
    currentTime += 50;
    if (!ledState && ((currentTime - lastStateChangeTime) >= onDuration))
    {
      ledState = true;
      lastStateChangeTime = currentTime;
    }

    if (ledState && ((currentTime - lastStateChangeTime)>= offDuration))
    {
      ledState = false;
      lastStateChangeTime = currentTime;
    }

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, ledState);
#endif
 }
//////////////////////////////////////////////
//
//
//             IO State Machine
//
//
//////////////////////////////////////////////
static sStateMachineType const gasTaskIO_Initializing[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
    {EVENT_IO_LED_INIT,       TaskIO_Start,                 TASKIO_IO_RUNNING,         		TASKIO_IO_INITIALIZING   	},
    {EVENT_IO_LED_NULL,       TaskIO_IgnoreEvent,       	TASKIO_IO_INITIALIZING,			TASKIO_IO_INITIALIZING		}
};

static sStateMachineType const gasTaskIO_Running[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
	{EVENT_IO_PS,       	  TaskIO_PSEvent,       		TASKIO_IO_SLEEPING,				TASKIO_IO_RUNNING		},
    {EVENT_IO_LED_NULL,       TaskIO_IgnoreEvent,       	TASKIO_IO_RUNNING,				TASKIO_IO_RUNNING		}
};

static sStateMachineType const gasTaskIO_Sleeping[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
	{EVENT_IO_WAKEUP,         TaskIO_WakeupEvent,       	TASKIO_IO_RUNNING,				TASKIO_IO_SLEEPING		},
    {EVENT_IO_LED_NULL,       TaskIO_IgnoreEvent,       	TASKIO_IO_SLEEPING,				TASKIO_IO_SLEEPING		}
};

static sStateMachineType const * const gpasTaskIO_StateMachine[] =
{
	gasTaskIO_Initializing,
	gasTaskIO_Running,
	gasTaskIO_Sleeping
};

void vTaskIO(void const * argument)
{
	if( xQueueReceive( xQueueIO, &stIOMsg, 0 ) )
	{
		(void)eEventHandler ((unsigned char)SRC_IO,gpasTaskIO_StateMachine[ucCurrentStateIO], &ucCurrentStateIO, &stIOMsg);
	}
}
