/********************************************************
 * DO NOT CHANGE THIS FILE, IT IS GENERATED AUTOMATICALY*
 ********************************************************/

/* Copyright 2008, 2009 Mariano Cerdeiro
 * Copyright 2014, ACSE & CADIEEL
 *      ACSE: http://www.sase.com.ar/asociacion-civil-sistemas-embebidos/ciaa/
 *      CADIEEL: http://www.cadieel.org.ar
 *
 * This file is part of CIAA Firmware.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** \brief FreeOSEK Os Generated Internal Configuration Implementation File
 **
 ** \file Os_Internal_Cfg.c
 **
 **/

/** \addtogroup FreeOSEK
 ** @{ */
/** \addtogroup FreeOSEK_Os
 ** @{ */
/** \addtogroup FreeOSEK_Os_Internal
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * MaCe         Mariano Cerdeiro
 * JuCe         Juan Cecconi
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 20141125 v0.1.3 JuCe additional stack for x86 ARCH
 * 20090719 v0.1.2 MaCe rename file to Os_
 * 20090128 v0.1.1 MaCe add OSEK_MEMMAP check
 * 20080713 v0.1.0 MaCe initial version
 */

/*==================[inclusions]=============================================*/
#include "Os_Internal.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/** \brief TaskUartReceive stack */
uint8 StackTaskTaskUartReceive[512];
/** \brief TaskLed stack */
uint8 StackTaskTaskLed[512];
/** \brief TaskIdle stack */
uint8 StackTaskTaskIdle[512];

/** \brief TaskUartReceive context */
TaskContextType ContextTaskTaskUartReceive;
/** \brief TaskLed context */
TaskContextType ContextTaskTaskLed;
/** \brief TaskIdle context */
TaskContextType ContextTaskTaskIdle;

/** \brief Ready List for Priority 2 */
TaskType ReadyList2[1];

/** \brief Ready List for Priority 1 */
TaskType ReadyList1[1];

/** \brief Ready List for Priority 0 */
TaskType ReadyList0[1];


/*==================[external data definition]===============================*/
/* FreeOSEK to configured priority table
 *
 * This table show the relationship between the user selected
 * priorities and the OpenOSE priorities:
 *
 * User P.			Osek P.
 * 5					2
 * 3					1
 * 1					0
 */

const TaskConstType TasksConst[TASKS_COUNT] = {
	/* Task TaskUartReceive */
	{
 		OSEK_TASK_TaskUartReceive,	/* task entry point */
		&ContextTaskTaskUartReceive, /* pointer to task context */
		StackTaskTaskUartReceive, /* pointer stack memory */
		512, /* stack size */
		1, /* task priority */
		1, /* task max activations */
		{
			1, /* extended task */
			1, /* preemtive task */
			0
		}, /* task const flags */
		0 , /* events mask */
		0 /* resources mask */
	},
	/* Task TaskLed */
	{
 		OSEK_TASK_TaskLed,	/* task entry point */
		&ContextTaskTaskLed, /* pointer to task context */
		StackTaskTaskLed, /* pointer stack memory */
		512, /* stack size */
		2, /* task priority */
		1, /* task max activations */
		{
			0, /* basic task */
			1, /* preemtive task */
			0
		}, /* task const flags */
		0 , /* events mask */
		0 /* resources mask */
	},
	/* Task TaskIdle */
	{
 		OSEK_TASK_TaskIdle,	/* task entry point */
		&ContextTaskTaskIdle, /* pointer to task context */
		StackTaskTaskIdle, /* pointer stack memory */
		512, /* stack size */
		0, /* task priority */
		1, /* task max activations */
		{
			1, /* extended task */
			1, /* preemtive task */
			0
		}, /* task const flags */
		0 , /* events mask */
		0 /* resources mask */
	}
};

/** \brief TaskVar Array */
TaskVariableType TasksVar[TASKS_COUNT];

/** \brief List of Auto Start Tasks in Application Mode AppMode1 */
const TaskType TasksAppModeAppMode1[2]  = {
	TaskUartReceive,
	TaskIdle
};
/** \brief AutoStart Array */
const AutoStartType AutoStart[1]  = {
	/* Application Mode AppMode1 */
	{
		2, /* Total Auto Start Tasks in this Application Mode */
		(TaskRefType)TasksAppModeAppMode1 /* Pointer to the list of Auto Start Stacks on this Application Mode */
	}
};

const ReadyConstType ReadyConst[3] = { 
	{
		1, /* Length of this ready list */
		ReadyList2 /* Pointer to the Ready List */
	},
	{
		1, /* Length of this ready list */
		ReadyList1 /* Pointer to the Ready List */
	},
	{
		1, /* Length of this ready list */
		ReadyList0 /* Pointer to the Ready List */
	}
};

/** TODO replace next line with: 
 ** ReadyVarType ReadyVar[3] ; */
ReadyVarType ReadyVar[3];

/** \brief Resources Priorities */
const TaskPriorityType ResourcesPriority[0]  = {

};
/** TODO replace next line with: 
 ** AlarmVarType AlarmsVar[0]; */
AlarmVarType AlarmsVar[0];

const AlarmConstType AlarmsConst[0]  = {

};

const AutoStartAlarmType AutoStartAlarm[ALARM_AUTOSTART_COUNT] = {

};

CounterVarType CountersVar[0];

const CounterConstType CountersConst[0] = {

};


/** TODO replace the next line with
 ** uint8 ApplicationMode; */
uint8 ApplicationMode;

/** TODO replace the next line with
 ** uint8 ErrorHookRunning; */
uint8 ErrorHookRunning;

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

