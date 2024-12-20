/*
 * TaskAppCAN.h
 *
 *  Created on: Nov 5, 2024
 *      Author: volkswagen2
 */

#ifndef INC_TASKAPPCAN_H_
#define INC_TASKAPPCAN_H_
#include "FreeRTOS.h"
#include "state.h"
#include "queue.h"
#include "gpio.h"
#include "can.h"
#include "cmsis_os.h"
#include "defines.h"

void TaskAppCAN_Entry(QueueHandle_t *xQueue,TimerHandle_t xTimer);
unsigned char TaskAppCAN_Start(sMessageType *psMessage);
unsigned char TaskAppCAN_ReceiveEvent(sMessageType *psMessage);
unsigned char TaskAppCAN_TransmitEvent(sMessageType *psMessage);
unsigned char TaskAppCAN_IgnoreEvent(sMessageType *psMessage);
void vLocalTimerCallbackAppCAN(void const * argument);
void vTaskAppCAN(void const * argument);

#endif /* INC_TASKAPPCAN_H_ */
