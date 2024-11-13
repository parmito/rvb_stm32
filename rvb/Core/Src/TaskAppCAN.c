/*
 * TaskAppCAN.c
 *
 *  Created on: Nov 5, 2024
 *      Author: volkswagen2
 */
#include "TaskAppCAN.h"
extern void vTimerCallbackAppCAN(void const * argument);
sMessageType stAppCANMsg;
static unsigned char ucCurrentStateAppCAN = TASK_APPCAN_INITIALIZING;
static QueueHandle_t xQueueAppCAN;
static TimerHandle_t xTimerAppCAN;
static QueueHandle_t *lQueueAppSerial;

static CAN_HandleTypeDef *hCAN;
static CAN_RxHeaderTypeDef pRxHeader;
static CAN_TxHeaderTypeDef pTxHeader;

typedef struct{
	uint32_t u32ID;
	uint32_t u32DLC;
	uint8_t u8Data[8];
}tstFrameCAN;
#define FIFO_SIZE 8
static uint8_t u8FifoRxCAN = 0;
static uint8_t u8FifoTxCAN = 0;
static tstFrameCAN stFrameRxCAN[8];
static tstFrameCAN stFrameTxCAN[8];
static uint16_t u16TimeSlice = 0;
static uint32_t u32TxMailbox = 0xFFFFFFFF;

//////////////////////////////////////////////
//
//
//              TaskAppCAN_Entry
//
//
//////////////////////////////////////////////
void TaskAppCAN_Entry(QueueHandle_t xQueue,TimerHandle_t xTimer)
{
    xQueueAppCAN = xQueue;
    xTimerAppCAN = xTimer;

    hCAN = hGet_CAN_Handler();

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPCAN;
    stAppCANMsg.ucEvent = EVENT_APPCAN_INIT;
    xQueueGenericSend(xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
}
//////////////////////////////////////////////
//
//
//              TaskAppCAN_Start
//
//
//////////////////////////////////////////////
unsigned char TaskAppCAN_Start(sMessageType *psMessage)
{
    unsigned char boError = true;

    (void)stHAL_CAN_FilterConfig();
    (void)HAL_CAN_Start(hCAN);
    lQueueAppSerial  = TaskAppSerial_getQueue();
	return boError;
}


//////////////////////////////////////////////
//
//
//              TaskAppCAN_ReceiveEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppCAN_ReceiveEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

    tstFrameCAN *pstFrameCAN = (tstFrameCAN*)(psMessage->pcMessageData);

	return boError;
}

//////////////////////////////////////////////
//
//
//              TaskAppCAN_TransmitEvent
//
//
//////////////////////////////////////////////
uint8_t u8String[24] = "DANILO MARTINS FRANCO\r\n";

unsigned char TaskAppCAN_TransmitEvent(sMessageType *psMessage)
{
    unsigned char boError = true;
    uint32_t u32;

    pTxHeader.StdId = 0x3C0;
    pTxHeader.IDE = CAN_ID_STD;
    pTxHeader.RTR = CAN_RTR_DATA;
    pTxHeader.DLC = 4;

    memset(stFrameTxCAN[u8FifoTxCAN].u8Data,0,sizeof(stFrameTxCAN[0].u8Data));

    stFrameTxCAN[u8FifoTxCAN].u8Data[0] = 0xC1;
    stFrameTxCAN[u8FifoTxCAN].u8Data[1] = 0x01;
    stFrameTxCAN[u8FifoTxCAN].u8Data[2] = 0x03;
    stFrameTxCAN[u8FifoTxCAN].u8Data[3] = 0x00;

	u32 = HAL_CAN_GetTxMailboxesFreeLevel(hCAN);
    if(u32 > 0)
    {
        (void)HAL_CAN_AddTxMessage(hCAN, &pTxHeader,stFrameTxCAN[u8FifoTxCAN].u8Data, &u32TxMailbox);
    }

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPSERIAL;
    stAppCANMsg.ucEvent = EVENT_APPSERIAL_TX;

    stAppCANMsg.pcMessageData = &u8String[0];

    xQueueGenericSend(lQueueAppSerial, &stAppCANMsg, 0,0);

	return boError;
}

//////////////////////////////////////////////
//
//
//              TaskAppCAN_IgnoreEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppCAN_IgnoreEvent(sMessageType *psMessage)
{
    unsigned char boError = false;

    return(boError);
}

//////////////////////////////////////////////
//
//
//              vTimerCallbackAppCAN
//
//
//////////////////////////////////////////////
void vTimerCallbackAppCAN(void const * argument)
 {

 }
//////////////////////////////////////////////
//
//
//             APP CAN State Machine
//
//
//////////////////////////////////////////////
static sStateMachineType const gasTaskAppCAN_Initializing[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
    {EVENT_APPCAN_INIT,       TaskAppCAN_Start,             TASK_APPCAN_RUNNING,         	TASK_APPCAN_INITIALIZING   	},
    {EVENT_APPCAN_NULL,       TaskAppCAN_IgnoreEvent,       TASK_APPCAN_INITIALIZING,		TASK_APPCAN_INITIALIZING		}
};

static sStateMachineType const gasTaskAppCAN_Running[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
    {EVENT_APPCAN_RX,       TaskAppCAN_ReceiveEvent,	   	TASK_APPCAN_RUNNING,			TASK_APPCAN_RUNNING		},
    {EVENT_APPCAN_TX,       TaskAppCAN_TransmitEvent,     	TASK_APPCAN_RUNNING,			TASK_APPCAN_RUNNING		},
    {EVENT_APPCAN_NULL,     TaskAppCAN_IgnoreEvent,        	TASK_APPCAN_RUNNING,			TASK_APPCAN_RUNNING		}
};

static sStateMachineType const * const gpasTaskAppCAN_StateMachine[] =
{
	gasTaskAppCAN_Initializing,
	gasTaskAppCAN_Running
};

static uint8_t u8TogglePin = 0;

void vTaskAppCAN(void const * argument)
{
	if( xQueueReceive(xQueueAppCAN, &stAppCANMsg, 0 ) )
	{
		(void)eEventHandler ((unsigned char)SRC_APPCAN,gpasTaskAppCAN_StateMachine[ucCurrentStateAppCAN], &ucCurrentStateAppCAN, &stAppCANMsg);
	}

	if( HAL_CAN_GetRxFifoFillLevel(hCAN, CAN_RX_FIFO0) > 0)
	{
		memset(stFrameRxCAN[u8FifoRxCAN].u8Data,0,sizeof(stFrameRxCAN[0].u8Data));
		if(HAL_CAN_GetRxMessage(hCAN, CAN_RX_FIFO0,&pRxHeader,stFrameRxCAN[u8FifoRxCAN].u8Data) == HAL_OK)
		{
			stAppCANMsg.ucSrc = SRC_APPCAN;
			stAppCANMsg.ucDest = SRC_APPCAN;
			stAppCANMsg.ucEvent = EVENT_APPCAN_RX;
			stFrameRxCAN[u8FifoRxCAN].u32ID = pRxHeader.StdId;
			stFrameRxCAN[u8FifoRxCAN].u32DLC = pRxHeader.DLC;
			stAppCANMsg.pcMessageData = (char*)&stFrameRxCAN[u8FifoRxCAN];
			xQueueGenericSend(xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
			if(++u8FifoRxCAN >= FIFO_SIZE)
			{
				u8FifoRxCAN = 0;
			}
		}
	}

	if(++u16TimeSlice >= 1000)
	{
		if(u8TogglePin)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		}
		u8TogglePin = !u8TogglePin;

		stAppCANMsg.ucSrc = SRC_APPCAN;
		stAppCANMsg.ucDest = SRC_APPCAN;
		stAppCANMsg.ucEvent = EVENT_APPCAN_TX;
		stAppCANMsg.pcMessageData = NULL;
		xQueueGenericSend(xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
		if(++u8FifoTxCAN >= FIFO_SIZE)
		{
			u8FifoTxCAN = 0;
		}
		u16TimeSlice = 0;
	}
}
