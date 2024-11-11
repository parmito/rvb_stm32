/*
 * TaskAppSerial.c
 *
 *  Created on: Nov 11, 2024
 *      Author: volkswagen2
 */
#include "TaskAppSerial.h"
extern void vTimerCallbackAppSerial(void const * argument);
sMessageType stAppSerialMsg;
static unsigned char ucCurrentStateAppSerial = TASK_APPSERIAL_INITIALIZING;
static QueueHandle_t *xQueueAppSerial;
static TimerHandle_t xTimerAppSerial;
static UART_HandleTypeDef *hUSART;

#define FIFO_SIZE 8
static uint8_t u8FrameRxUSART[128];
static uint8_t u8FrameTxUSART[128];
static uint8_t u8FifoRxUSART = 0;
static uint8_t u8FifoTxUSART = 0;
static uint16_t u16TimeSlice = 0;


//////////////////////////////////////////////
//
//
//              TaskAppSerial_Entry
//
//
//////////////////////////////////////////////
void TaskAppSerial_Entry(QueueHandle_t *xQueue,TimerHandle_t xTimer)
{
    xQueueAppSerial = xQueue;
    xTimerAppSerial = xTimer;

    hUSART = hGet_USART_Handler();

    stAppSerialMsg.ucSrc = SRC_APPSERIAL;
    stAppSerialMsg.ucDest = SRC_APPSERIAL;
    stAppSerialMsg.ucEvent = EVENT_APPSERIAL_INIT;
    xQueueGenericSend(*xQueueAppSerial, ( void * )&stAppSerialMsg, 0,0);
}
//////////////////////////////////////////////
//
//
//              TaskAppSerial_Start
//
//
//////////////////////////////////////////////
unsigned char TaskAppSerial_Start(sMessageType *psMessage)
{
    unsigned char boError = true;


	return boError;
}


//////////////////////////////////////////////
//
//
//              TaskAppSerial_ReceiveEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppSerial_ReceiveEvent(sMessageType *psMessage)
{
    unsigned char boError = true;


	return boError;
}

//////////////////////////////////////////////
//
//
//              TaskAppSerial_TransmitEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppSerial_TransmitEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

    uint8_t u8String[] = "Danilo Franco";

    memcpy(u8FrameTxUSART,u8String,sizeof(u8String));

    (void)HAL_UART_Transmit(hUSART, u8FrameTxUSART, sizeof(u8String),10);

	return boError;
}

//////////////////////////////////////////////
//
//
//              TaskAppSerial_IgnoreEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppSerial_IgnoreEvent(sMessageType *psMessage)
{
    unsigned char boError = false;

    return(boError);
}

//////////////////////////////////////////////
//
//
//              vTimerCallbackAppSerial
//
//
//////////////////////////////////////////////
void vTimerCallbackAppSerial(void const * argument)
 {

 }
//////////////////////////////////////////////
//
//
//             APP CAN State Machine
//
//
//////////////////////////////////////////////
static sStateMachineType const gasTaskAppSerial_Initializing[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
    {EVENT_APPSERIAL_INIT,       TaskAppSerial_Start,             TASK_APPSERIAL_RUNNING,         	TASK_APPSERIAL_INITIALIZING   	},
    {EVENT_APPSERIAL_NULL,       TaskAppSerial_IgnoreEvent,       TASK_APPSERIAL_INITIALIZING,		TASK_APPSERIAL_INITIALIZING		}
};

static sStateMachineType const gasTaskAppSerial_Running[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
    {EVENT_APPSERIAL_RX,       TaskAppSerial_ReceiveEvent,	   		TASK_APPSERIAL_RUNNING,			TASK_APPSERIAL_RUNNING		},
    {EVENT_APPSERIAL_TX,       TaskAppSerial_TransmitEvent,     	TASK_APPSERIAL_RUNNING,			TASK_APPSERIAL_RUNNING		},
    {EVENT_APPSERIAL_NULL,     TaskAppSerial_IgnoreEvent,        	TASK_APPSERIAL_RUNNING,			TASK_APPSERIAL_RUNNING		}
};

static sStateMachineType const * const gpasTaskAppSerial_StateMachine[] =
{
	gasTaskAppSerial_Initializing,
	gasTaskAppSerial_Running
};

void vTaskAppSerial(void const * argument)
{
	if( xQueueReceive( *xQueueAppSerial, &stAppSerialMsg, 0 ) )
	{
		(void)eEventHandler ((unsigned char)SRC_APPSERIAL,gpasTaskAppSerial_StateMachine[ucCurrentStateAppSerial], &ucCurrentStateAppSerial, &stAppSerialMsg);
	}

	memset(u8FrameRxUSART,0,sizeof(u8FrameRxUSART));
	if(HAL_UART_Receive(hUSART, u8FrameRxUSART, sizeof(u8FrameRxUSART),0) == HAL_OK)
	{
		stAppSerialMsg.ucSrc = SRC_APPSERIAL;
		stAppSerialMsg.ucDest = SRC_APPSERIAL;
		stAppSerialMsg.ucEvent = EVENT_APPSERIAL_RX;
		stAppSerialMsg.pcMessageData = (char*)&u8FrameRxUSART;
		xQueueGenericSend(*xQueueAppSerial, ( void * )&stAppSerialMsg, 0,0);
	}

	if(++u16TimeSlice >= 1000)
	{
		stAppSerialMsg.ucSrc = SRC_APPSERIAL;
		stAppSerialMsg.ucDest = SRC_APPSERIAL;
		stAppSerialMsg.ucEvent = EVENT_APPSERIAL_TX;
		stAppSerialMsg.pcMessageData = NULL;
		xQueueGenericSend(*xQueueAppSerial, ( void * )&stAppSerialMsg, 0,0);
		u16TimeSlice = 0;
	}
}


