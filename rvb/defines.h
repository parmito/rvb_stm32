#ifndef _DEFINES_H
#define _DEFINES_H

#include "stdint.h"
#ifdef __cplusplus
 extern "C" {
#endif


#define false   (char)(0)
#define true 	(char)(1)

#define NUM_QUEUE 1
#define NUM_TASKS 2

#define DEBUG_IO					1
#define DEBUG_APPCAN				1

#define TASK_IO_TIMEOUT				(unsigned long)(25)
#define TASK_APPCAN_TIMEOUT 		(unsigned long)(50)

#define TASK_IO_1_SECOND			(unsigned long)(1000/TASK_IO_TIMEOUT)
#define TASK_APPCAN_1_SECOND		(unsigned long)(1000/TASK_APPCAN_TIMEOUT)

/* Events*/
#define EVENT_IO_LED_INIT 	        (unsigned char)0
#define EVENT_IO_LED_ON	 	        (unsigned char)1
#define	EVENT_IO_LED_FLASHING		(unsigned char)2
#define	EVENT_IO_LED_OFF			(unsigned char)3
#define	EVENT_IO_LED_NULL			(unsigned char)4

#define EVENT_APPCAN_INIT 	        (unsigned char)0
#define EVENT_APPCAN_RX				(unsigned char)1
#define EVENT_APPCAN_TX 			(unsigned char)2
#define	EVENT_APPCAN_NULL			(unsigned char)3

/* States*/
#define TASKIO_IO_INITIALIZING    	(unsigned char)0
#define TASKIO_IO_FLASHING	    	(unsigned char)1
#define TASKIO_IO_SLEEPING	    	(unsigned char)2

#define TASK_APPCAN_INITIALIZING 	(unsigned char)0
#define TASK_APPCAN_WORKING	 	(unsigned char)1
#define TASK_APPCAN_SLEEPING    	(unsigned char)2

 /* Timers*/
#define TIMER_IO_FLASHING			(unsigned char)0
#define TIMER_APPCAN_FLASHING		(unsigned char)0
/* Installed sources*/
#define SRC_IO     		1
#define SRC_APPCAN   	1



 /* The number of items the queue can hold.  This is 1 as the receive task
 will remove items as they are added, meaning the send task should always find
 the queue empty. */

#define ioQUEUE_LENGTH						( 5 )


typedef struct
{
 	unsigned char ucIgnition;
 	unsigned int u16AdMainBatteryVoltage;
 	float flAdMainBatteryVoltage;
 	unsigned int u16AdBackupBatteryVoltage;
 	float flAdBackupBatteryVoltage;
 	float flI2cTemperature;
 	unsigned int u16IntTemperature;
}tstIo;

typedef struct
{
    unsigned char   ucSrc;
    unsigned char   ucDest;
    unsigned char   ucEvent;
    char*           pcMessageData;
}sMessageType;

typedef struct
{
    /* event constructed from message  */
    unsigned char ucEvent;
    /* action routine invoked for the event */
    unsigned char (*ActionFun) (sMessageType *psMessage);
    /* next state in case of success*/
    unsigned char ucStateSuccess;
    /* next state in case of failure*/
    unsigned char ucStateFailure;
}sStateMachineType;


#define DIAG_LED1_EN_PIN        		(uint16_t)GPIO_Pin_11
#define DIAG_LED1_EN_GPIO        		(GPIO_TypeDef*)GPIOA
#define DIAG_LED1_EN(x)          		GPIO_WriteBit(DIAG_LED1_EN_GPIO, DIAG_LED1_EN_PIN, x)
#define DIAG_LED1_TOGGLE				((GPIO_ReadOutputDataBit(DIAG_LED1_EN_GPIO, DIAG_LED1_EN_PIN))? (DIAG_LED1_EN(Bit_RESET)): (DIAG_LED1_EN(Bit_SET)))


/* IO MAPPING*/
#define DIAG_LED1			PA11

#ifdef __cplusplus
}
#endif
#endif
