/*
 * TaskAppSerial.h
 *
 *  Created on: Nov 11, 2024
 *      Author: volkswagen2
 */

#ifndef INC_TASKAPPSERIAL_H_
#define INC_TASKAPPSERIAL_H_

#include "FreeRTOS.h"
#include "state.h"
#include "queue.h"
#include "gpio.h"
#include "usart.h"
#include "cmsis_os.h"
#include "defines.h"
#include <string.h>
#include <stdio.h>


void TaskAppSerial_Entry(QueueHandle_t xQueue,TimerHandle_t xTimer);
unsigned char TaskAppSerial_Start(sMessageType *psMessage);
unsigned char TaskAppSerial_ReceiveEvent(sMessageType *psMessage);
unsigned char TaskAppSerial_TransmitEvent(sMessageType *psMessage);
unsigned char TaskAppSerial_IgnoreEvent(sMessageType *psMessage);
void vLocalTimerCallbackAppSerial(void const * argument);
void vTaskAppSerial(void const * argument);
QueueHandle_t *TaskAppSerial_getQueue(void);
/*uint8_t *TaskAppSerial_getTxBuffer(void);*/
uint16_t TaskAppSerial_getTxBuffer(const uint8_t *pu8Buffer);

#endif /* INC_TASKAPPSERIAL_H_ */
