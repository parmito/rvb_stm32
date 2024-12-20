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
static QueueHandle_t *pstQueueAppSerial;
static QueueHandle_t *pstQueueIO;

static CAN_HandleTypeDef *hCAN;
static CAN_RxHeaderTypeDef pRxHeader;
/*static CAN_TxHeaderTypeDef pTxHeader;*/

typedef struct{
	uint32_t u32DID;
	uint32_t u32DLC;
	uint8_t u8Data[8];
}tstFrameCAN;

static uint8_t u8FifoRxCAN = 0;
/*static uint8_t u8FifoTxCAN = 0;*/
static tstFrameCAN stFrameRxCAN[8];
/*static tstFrameCAN stFrameTxCAN[8];*/
/*static uint16_t u16TimeSlice = 0;*/
/*static uint32_t u32TxMailbox = 0xFFFFFFFF;*/

static uint8_t u8SerialTxBuffer[64];
static uint16_t u16TimeToSleep = 0;

//////////////////////////////////////////////
//
//
//     HAL_CAN_RxFifo0MsgPendingCallback
//
//
//////////////////////////////////////////////

extern void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	HAL_ResumeTick();

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPCAN;
    stAppCANMsg.ucEvent = EVENT_APPCAN_WAKEUP;
    stAppCANMsg.pcMessageData = NULL;
    xQueueGenericSendFromISR(xQueueAppCAN, &stAppCANMsg, 0,0);


}

//////////////////////////////////////////////
//
//
//              TaskAppCAN_getQueue
//
//
//////////////////////////////////////////////
QueueHandle_t *TaskAppCAN_getQueue()
{
	return &xQueueAppCAN;
}


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

    pstQueueAppSerial  = TaskAppSerial_getQueue();
    pstQueueIO = TaskIO_getQueue();
    /*u16SizeSerialTxBuffer =  TaskAppSerial_getTxBuffer(&u8SerialTxBuffer);
    pu8SerialTxBuffer = &u8SerialTxBuffer;*/
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

    /*if(((uint32_t)pstFrameCAN->u32DID == 0x03C0) && ((pstFrameCAN->u8Data[2] & 0x03) == 0))
    {
        HAL_SuspendTick();
        HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
        HAL_CAN_ActivateNotification(hCAN, CAN_IT_RX_FIFO0_MSG_PENDING);
    	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }*/

    memset((char*)u8SerialTxBuffer,0x00,sizeof(u8SerialTxBuffer));
    sprintf((char*)&u8SerialTxBuffer[0],"%8X    ",(uint32_t)pstFrameCAN->u32DID);
    size_t u16Len = strlen((char*)&u8SerialTxBuffer[0]);
    for(uint32_t u32 = 0; u32 < pstFrameCAN->u32DLC; u32++)
    {
    	sprintf((char*)&u8SerialTxBuffer[u16Len],"%02X ",(uint16_t)pstFrameCAN->u8Data[u32]);
    	u16Len = strlen((char*)&u8SerialTxBuffer[0]);
    }
	u16Len = strlen((char*)&u8SerialTxBuffer[0]);
	sprintf((char*)&u8SerialTxBuffer[u16Len],"\r\n");

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPSERIAL;
    stAppCANMsg.ucEvent = EVENT_APPSERIAL_TX;
    stAppCANMsg.pcMessageData = (char*)&u8SerialTxBuffer[0];
    xQueueGenericSend(*pstQueueAppSerial, &stAppCANMsg, 0,0);

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
/*    uint32_t u32;

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
    }*/

    /*stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPSERIAL;
    stAppCANMsg.ucEvent = EVENT_APPSERIAL_TX;
    strcpy((char*)pu8SerialTxBuffer,"SURDAO\r\n");
    stAppCANMsg.pcMessageData = (char*)pu8SerialTxBuffer;
    xQueueGenericSend(*pstQueueAppSerial, &stAppCANMsg, 0,0);*/

	return boError;
}

//////////////////////////////////////////////
//
//
//             TaskAppCAN_PSEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppCAN_PSEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_IO;
    stAppCANMsg.ucEvent = EVENT_IO_PS;
    stAppCANMsg.pcMessageData = NULL;
    xQueueGenericSend(*pstQueueIO, &stAppCANMsg, 0,0);

	return boError;
}
uint16_t VirtAddVarTab[NB_OF_VAR];
uint16_t VarDataTab[NB_OF_VAR] = {'D', 'a', 'n', 'i', 'l', 'o'};
uint8_t VarDataTabRead[NB_OF_VAR];
uint16_t VarIndex,VarDataTmp = 0;
//////////////////////////////////////////////
//
//
//             TaskAppCAN_SleepEvent
//
//
//////////////////////////////////////////////
unsigned char TaskAppCAN_SleepEvent(sMessageType *psMessage)
{
    unsigned char boError = true;

	HAL_SuspendTick();
	/*Configure GPIO pin Output Level */

	uint16_t VirtAddVarTab[NB_OF_VAR];
	uint16_t VarDataTab[NB_OF_VAR] = {'C','a','r','l','o','s'};
	uint8_t VarDataTabRead[NB_OF_VAR];
	uint16_t VarIndex,VarDataTmp = 0;

	HAL_FLASH_Unlock();

	/* EEPROM Init */
	if( EE_Init() != HAL_OK)
	{
		Error_Handler();
	}

	/*EE_Format();*/

	for(VarIndex = 1; VarIndex <= NB_OF_VAR; VarIndex++)
	{
		VirtAddVarTab[VarIndex-1] = VarIndex;
	}

	// Fill EEPROM variables addresses

	for(VarIndex = 1; VarIndex <= NB_OF_VAR; VarIndex++)
	{
		VirtAddVarTab[VarIndex-1] = VarIndex;
	}
	for (VarIndex = 0; VarIndex < NB_OF_VAR; VarIndex++)
	{
		if((EE_ReadVariable(VirtAddVarTab[VarIndex],  &VarDataTabRead[VarIndex])) != HAL_OK)
		{
			Error_Handler();
		}
	}


	// Fill EEPROM variables addresses
	for(VarIndex = 1; VarIndex <= NB_OF_VAR; VarIndex++)
	{
		VirtAddVarTab[VarIndex-1] = VarIndex;
	}

	for (VarIndex = 0; VarIndex < NB_OF_VAR; VarIndex++)
	{
		/* Sequence 1 */
		if((EE_WriteVariable(VirtAddVarTab[VarIndex],  VarDataTab[VarIndex])) != HAL_OK)
		{
			Error_Handler();
		}
	}

	for (VarIndex = 0; VarIndex < NB_OF_VAR; VarIndex++)
	{
		if((EE_ReadVariable(VirtAddVarTab[VarIndex],  &VarDataTabRead[VarIndex])) != HAL_OK)
		{
			Error_Handler();
		}
	}


	HAL_CAN_Stop(hCAN);
	HAL_CAN_MspDeInit(hCAN);
	HAL_CAN_MspInit(hCAN);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	HAL_CAN_Start(hCAN);
    HAL_CAN_ActivateNotification(hCAN, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	return boError;
}

//////////////////////////////////////////////
//
//
//             TaskAppCAN_WakeUp
//
//
//////////////////////////////////////////////
unsigned char TaskAppCAN_WakeUp(sMessageType *psMessage)
{
    unsigned char boError = true;

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_IO;
    stAppCANMsg.ucEvent = EVENT_IO_WAKEUP;
    stAppCANMsg.pcMessageData = NULL;
    xQueueGenericSend(*pstQueueIO, &stAppCANMsg, 0,0);

    stAppCANMsg.ucSrc = SRC_APPCAN;
    stAppCANMsg.ucDest = SRC_APPCAN;
    stAppCANMsg.ucEvent = EVENT_APPCAN_INIT;
    xQueueGenericSend(xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);

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
    {EVENT_APPCAN_PS,		TaskAppCAN_PSEvent, 			TASK_APPCAN_PS,					TASK_APPCAN_RUNNING		},
    {EVENT_APPCAN_NULL,     TaskAppCAN_IgnoreEvent,        	TASK_APPCAN_RUNNING,			TASK_APPCAN_RUNNING		}
};

static sStateMachineType const gasTaskAppCAN_PS[] =
{
    /* Event        Action routine      Next state */
    /*  State specific transitions	*/
    {EVENT_APPCAN_SLEEP,     	TaskAppCAN_SleepEvent,        	TASK_APPCAN_PS,				TASK_APPCAN_PS			},
    {EVENT_APPCAN_WAKEUP,     	TaskAppCAN_WakeUp,        		TASK_APPCAN_INITIALIZING,	TASK_APPCAN_PS			},
    {EVENT_APPCAN_NULL,     	TaskAppCAN_IgnoreEvent,        	TASK_APPCAN_PS,				TASK_APPCAN_PS			}
};


static sStateMachineType const * const gpasTaskAppCAN_StateMachine[] =
{
	gasTaskAppCAN_Initializing,
	gasTaskAppCAN_Running,
	gasTaskAppCAN_PS
};

/*static uint8_t u8TogglePin = 0;*/

void vTaskAppCAN(void const * argument)
{
	if( HAL_CAN_GetRxFifoFillLevel(hCAN, CAN_RX_FIFO0) > 0)
	{
		u16TimeToSleep = 0;

		memset(&pRxHeader,0x00,sizeof(CAN_RxHeaderTypeDef));
		memset(stFrameRxCAN[u8FifoRxCAN].u8Data,0xFF,sizeof(tstFrameCAN));

		if(HAL_CAN_GetRxMessage(hCAN, CAN_RX_FIFO0,&pRxHeader,stFrameRxCAN[u8FifoRxCAN].u8Data) == HAL_OK)
		{
			stFrameRxCAN[u8FifoRxCAN].u32DID = pRxHeader.StdId;
			if(pRxHeader.ExtId != 0)
			{
				stFrameRxCAN[u8FifoRxCAN].u32DID = pRxHeader.ExtId;
			}
			stFrameRxCAN[u8FifoRxCAN].u32DLC = pRxHeader.DLC;

			stAppCANMsg.ucSrc = SRC_APPCAN;
			stAppCANMsg.ucDest = SRC_APPCAN;
			stAppCANMsg.ucEvent = EVENT_APPCAN_RX;
			stAppCANMsg.pcMessageData = (char*)&stFrameRxCAN[u8FifoRxCAN];
			if(++u8FifoRxCAN >= FIFO_SIZE)
			{
				u8FifoRxCAN = 0;
			}
			xQueueGenericSend(xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
		}
	}
	else
	{
		if(++u16TimeToSleep >= 1000)
		{
			u16TimeToSleep = 0;
			stAppCANMsg.ucSrc = SRC_APPCAN;
			stAppCANMsg.ucDest = SRC_APPCAN;
			stAppCANMsg.ucEvent = EVENT_APPCAN_PS;
			stAppCANMsg.pcMessageData = NULL;
			xQueueGenericSend(xQueueAppCAN, ( void * )&stAppCANMsg, 0,0);
		}
	}

	if( xQueueReceive(xQueueAppCAN, &stAppCANMsg, 0 ) )
	{
		(void)eEventHandler ((unsigned char)SRC_APPCAN,gpasTaskAppCAN_StateMachine[ucCurrentStateAppCAN], &ucCurrentStateAppCAN, &stAppCANMsg);
	}
}

