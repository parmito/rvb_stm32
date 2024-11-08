/*
 * state.c
 *
 *  Created on: Oct 16, 2017
 *      Author: Danilo_2
 */

#include "state.h"


const sStateMachineType *psSearchEvent (const sStateMachineType *psStateTable,unsigned char ucIncoming)
{
    unsigned char ucEvent;

    for (;; psStateTable++)
    {
        ucEvent = psStateTable->ucEvent;
        if ((ucEvent != 0)&&(ucEvent != ucIncoming))
        {
           continue;
        }
        return psStateTable;
    }

}

void eEventHandler (unsigned char ucDest,const sStateMachineType *psStateTable, unsigned char *piState, sMessageType *psMessage)
{
    unsigned char eError = 0;

    if(ucDest == psMessage->ucDest)
    {
        psStateTable = psSearchEvent(psStateTable, psMessage->ucEvent);

        eError = (*psStateTable->ActionFun)(psMessage);

        if (eError == 1)
        {
            *piState = psStateTable->ucStateSuccess;
        }
        else
        {
            *piState = psStateTable->ucStateFailure;
        }
    }
}
