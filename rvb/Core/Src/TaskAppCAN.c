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
static QueueHandle_t *xQueueAppCAN;
static TimerHandle_t xTimerAppCAN;


static uint32_t ulCountPulse;
static uint32_t ulCountTimePulseOn;
static uint32_t ulCountTimePulseOff;
static uint32_t ulCountPeriod;

static uint32_t ulQtyPulse;
static uint32_t ulTimePulseOn;
static uint32_t ulTimePulseOff;
static uint32_t ulPeriod;

static CAN_HandleTypeDef *hCAN;
static CAN_RxHeaderTypeDef pRxHeader;

typedef struct{
	uint32_t u32ID;
	uint32_t u32DLC;
	uint8_t u8Data[8];
}tstFrameCAN;
#define FIFO_SIZE 8
static uint8_t u8FifoCAN = 0;
static tstFrameCAN stFrameCAN[8];
static uint16_t u16TimeSlice = 0;

//////////////////////////////////////////////
//
//
//              TaskAppCAN_Entry
//
//
//////////////////////////////////////////////
void TaskAppCAN_Entry(QueueHandle_t *xQueue,TimerHandle_t xTimer)
{
    xQueueAppCAN = xQueue;
    xTimerAppCAN = xTimer;

    hCAN = hGet_CAN_Handler();

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPCAN;
    stAppCANMsg.ucEvent = EVENT_APPCAN_INIT;
    xQueueGenericSend(*xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
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
    HAL_StatusTypeDef enStatusStart,enStatusFilter;
    enStatusFilter = stHAL_CAN_FilterConfig();
    enStatusStart = HAL_CAN_Start(hCAN);

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
unsigned char TaskAppCAN_TransmitEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

    HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader,
                                           const uint8_t aData[], uint32_t *pTxMailbox)

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

void vTaskAppCAN(void const * argument)
{
	if( xQueueReceive( *xQueueAppCAN, &stAppCANMsg, 0 ) )
	{
		(void)eEventHandler ((unsigned char)SRC_APPCAN,gpasTaskAppCAN_StateMachine[ucCurrentStateAppCAN], &ucCurrentStateAppCAN, &stAppCANMsg);
	}

	if( HAL_CAN_GetRxFifoFillLevel(hCAN, CAN_RX_FIFO0) > 0)
	{
		memset(stFrameCAN[u8FifoCAN].u8Data,0,sizeof(stFrameCAN[0].u8Data));
		if(HAL_CAN_GetRxMessage(hCAN, CAN_RX_FIFO0,&pRxHeader,stFrameCAN[u8FifoCAN].u8Data) == HAL_OK)
		{
			stAppCANMsg.ucSrc = SRC_APPCAN;
			stAppCANMsg.ucDest = SRC_APPCAN;
			stAppCANMsg.ucEvent = EVENT_APPCAN_RX;
			stFrameCAN[u8FifoCAN].u32ID = pRxHeader.StdId;
			stFrameCAN[u8FifoCAN].u32DLC = pRxHeader.DLC;
			stAppCANMsg.pcMessageData = (char*)&stFrameCAN[u8FifoCAN];
			xQueueGenericSend(*xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
			if(++u8FifoCAN >= FIFO_SIZE)
			{
				u8FifoCAN = 0;
			}
		}
	}

	if(++u16TimeSlice >= 100)
	{
		stAppCANMsg.ucSrc = SRC_APPCAN;
		stAppCANMsg.ucDest = SRC_APPCAN;
		stAppCANMsg.ucEvent = EVENT_APPCAN_TX;
		stAppCANMsg.pcMessageData = NULL;
		xQueueGenericSend(*xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);

		u16TimeSlice = 0;
	}
}

