/*
 * MyNewTask.c
 *
 *  Created on: Oct 27, 2020
 *      Author: nxf63348
 */
#include "MyNewTask.h"

osaEventId_t mMyEvents;
/*Global variable to store our TimerID*/
tmrTimerID_t myTmrID = gTmrInvalidTimerID_c;

/*Handler ID for task*/
osaTaskId_t gMyTaskHandler_ID;

/* Local variable to store the current state of the LEDs*/
static uint8_t ledsState = 0;

/*OSA Task definition*/
OSA_TASK_DEFINE(My_Task, gMyTaskPriority_c, 1, gMyTaskStackSize_c, FALSE);
static uint8_t ledsCounter = 0;
static uint8_t ledsCounter_next = 0;

static void myTaskTimerCallback(void *param)
{
OSA_EventSet(mMyEvents, gMyNewTaskEvent2_c);
}

void led_change(uint8_t* Counter)
{
	uint8_t ledCounter = Counter[8]-48;
	switch (ledCounter){
	case 0:
		TurnOffLeds();
		Led_TurnOn(GREEN);
		break;
	case 1:
		TurnOffLeds();
		Led_TurnOn(RED);
		break;
	case 2:
		TurnOffLeds();
		Led_TurnOn(BLUE);
		break;
	case 3:
		TurnOffLeds();
		TurnOnLeds();
		break;
	default:
		TurnOffLeds();
		break;
	}

}

/*Main custom stack*/
void My_Task(osaTaskParam_t argument)
{
	osaEventFlags_t customEvent;
	myTmrID = TMR_AllocateTimer();

	while(1)
	{
		OSA_EventWait(mMyEvents, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &customEvent);
		if(!gUseRtos_c && !customEvent)
		{
			break;
		}

		/* Depending on the received event*/
		switch(customEvent)
		{
		case gMyNewTaskEvent1_c:
			TMR_StartIntervalTimer(myTmrID, 3000, myTaskTimerCallback, NULL);
			TurnOffLeds();
			break;
		case gMyNewTaskEvent2_c:
			ledsCounter = ledsCounter_next;
			switch (ledsCounter){
				case 0:
					TurnOffLeds();
					Led_TurnOn(RED);
					ledsCounter_next = 1;
					break;
				case 1:
					TurnOffLeds();
					Led_TurnOn(GREEN);
					ledsCounter_next = 2;
					break;
				case 2:
					TurnOffLeds();
					Led_TurnOn(BLUE);
					ledsCounter_next = 3;
					break;
				case 3:
					TurnOffLeds();
					TurnOnLeds();
					ledsCounter_next = 0;
					break;
				default:
					TurnOffLeds();
					ledsCounter_next = 0;
					break;
			}
			break;
		case gMyNewTaskEvent3_c: /* Event to stop the timer */
			ledsCounter = 0;
			TurnOffLeds();
			TMR_StopTimer(myTmrID);
			break;
		default:
			break;

		}
	}
}

void MyTask_Init(void)
{
	mMyEvents = OSA_EventCreate(TRUE);
	/*The instance of the MAC is passed at task creation*/
	gMyTaskHandler_ID = OSA_TaskCreate(OSA_TASK(My_Task), NULL);
}

int MyTask_GetCounter(void)
{
	return ledsCounter;
}
/*This is the funciton called by the Timer each time it expires*/
static void myTaskTimerCallback(void *param)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent2_c);
}

/*Public function to send an event to stop the timer*/
void MyTaskTimer_Stop(void)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent3_c);
}

void MyTaskTimer_Start(void)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent1_c);
}


