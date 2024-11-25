/*
 * TaskIO.h
 *
 *  Created on: Nov 1, 2024
 *      Author: volkswagen2
 */

#ifndef INC_TASKIO_H_
#define INC_TASKIO_H_
#include "FreeRTOS.h"
#include "state.h"
#include "queue.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "defines.h"

void TaskIO_Entry(QueueHandle_t *xQueue,TimerHandle_t xTimer);
unsigned char TaskIO_Start(sMessageType *psMessage);
unsigned char TaskIO_IgnoreEvent(sMessageType *psMessage);
void vLocalTimerCallbackIo(void const * argument);
void vTaskIO(void const * argument);

#endif /* INC_TASKIO_H_ */
