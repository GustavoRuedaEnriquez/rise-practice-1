/*
 * MyNewTask.c
 */

#include "MyNewTask.h"

osaEventId_t mMyEvents;
/* Global Variable to store our TimerID */
tmrTimerID_t myTimerID = gTmrInvalidTimerID_c;
/* Handler ID for task */
osaTaskId_t gMyTaskHandler_ID;
/* Local variable to store the current state of the LEDs */
static uint8_t ledsState = 0;

uint8_t counter = 1;

/* OSA Task Definition*/
OSA_TASK_DEFINE(My_Task, gMyTaskPriority_c, 1, gMyTaskStackSize_c, FALSE );

static void myTaskTimerCallback(void *param);
static nwkToMcpsMessage_t *mpPacket;

/* Main custom task */
void My_Task(osaTaskParam_t argument)
{
	osaEventFlags_t customEvent;
	myTimerID = TMR_AllocateTimer();

	while (1)
	{
		OSA_EventWait(mMyEvents, osaEventFlagsAll_c, FALSE, osaWaitForever_c,
				&customEvent);
		if (!gUseRtos_c && !customEvent)
		{
			break;
		}
		/* Depending on the received event */
		switch (customEvent)
		{
		case gMyNewTaskEvent1_c:

			if(counter > 4)
			{
				counter = 1;
			}

			TurnOffLeds();
			switch(counter)
			{
			case 1:
				Led1On();
				break;
			case 2:
				Led2On();
				break;
			case 3:
				Led3On();
				break;
			case 4:
				led4On();
				break;
			default:
				break;
			}

			mpPacket = MSG_Alloc(sizeof(nwkToMcpsMessage_t) + 1);

		    if(mpPacket != NULL)
		    {
		    	mpPacket->msgType = gMcpsDataReq_c;
		        mpPacket->msgData.dataReq.pMsdu = (uint8_t*)(&mpPacket->msgData.dataReq.pMsdu) +
		            sizeof(mpPacket->msgData.dataReq.pMsdu);
		        mpPacket->msgData.dataReq.pMsdu = &counter;

		    }
			break;
			counter++;
		case gMyNewTaskEvent2_c: /* Event called from myTaskTimerCallback */
			if (!ledsState)
			{
				TurnOnLeds();
				ledsState = 1;
			} else
			{
				TurnOffLeds();
				ledsState = 0;
			}
			break;
		case gMyNewTaskEvent3_c: /* Event to stop the timer */
			ledsState = 0;
			TurnOffLeds();
			TMR_StopTimer(myTimerID);
			break;
		default:
			break;
		}
	}
}

/* Function to init the task */
void MyTask_Init(void)
{
	mMyEvents = OSA_EventCreate(TRUE);
	/* The instance of the MAC is passed at task creaton */
	gMyTaskHandler_ID = OSA_TaskCreate(OSA_TASK(My_Task), NULL);
}


/* This is the function called by the Timer each time it expires */
static void myTaskTimerCallback(void *param)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent1_c);
}

/* Public function to send an event to stop the timer */
void MyTaskTimer_Stop(void)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent3_c);
}

void MyTaskTimer_Start(void)
{
	TMR_StartIntervalTimer(myTimerID,3000,(pfTmrCallBack_t)myTaskTimerCallback,NULL);
}



