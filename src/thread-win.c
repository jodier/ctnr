/* Author  : Jerome ODIER
 * Email   : jerome.odier@cern.ch
 *
 * Version : 1.0 (2007-2012)
 *
 *
 * This file is part of LIBCTNR.
 *
 *  Foobar is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <windows.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/
/* THREAD								   */
/*-------------------------------------------------------------------------*/

typedef struct my_thread_s
{
	HANDLE thread;

} my_thread_t;

/*-------------------------------------------------------------------------*/

void ctnr_thread_create(ctnr_thread_t *thread, void *(* func)(void *), void *arg)
{
	my_thread_t *object = ctnr_cast(my_thread_t *, thread->backend);

	/**/

	object->thread = CreateThread(NULL, 0, ctnr_cast(LPTHREAD_START_ROUTINE, func), arg, 0, NULL);

	if(object->thread == NULL)
	{
		ctnr_panic("WINDOW$ thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void *ctnr_thread_waitfor(ctnr_thread_t *thread)
{
	my_thread_t *object = ctnr_cast(my_thread_t *, thread->backend);

	/**/

	DWORD status = WaitForSingleObject(object->thread, INFINITE);

	if((status != WAIT_OBJECT_0
	    &&
	    status != WAIT_ABANDONED
	   )
	   || GetExitCodeThread(object->thread, &status) == FALSE || CloseHandle(object->thread) == FALSE
	 ) {
		ctnr_panic("WINDOW$ thread error !\n");
	}

	return ctnr_cast(void *, ctnr_cast(uintptr_t, status));
}

/*-------------------------------------------------------------------------*/

void ctnr_thread_exit(void *ret)
{
	ExitThread(ctnr_cast(uintptr_t, ret));

	exit(-1);
}

/*-------------------------------------------------------------------------*/
/* MUTEX								   */
/*-------------------------------------------------------------------------*/

#define EnterCriticalSection(handle) \
		DWORD CTNR_SYMB(status) = WaitForSingleObject(handle, INFINITE);	\
											\
		if(CTNR_SYMB(status) != WAIT_OBJECT_0					\
		   &&									\
		   CTNR_SYMB(status) != WAIT_ABANDONED					\
		 ) {									\
			ctnr_panic("WINDOW$ thread error !\n");				\
		}

/*-------------------------------------------------------------------------*/

#define LeaveCriticalSection(handle) \
		if(ReleaseMutex(handle) == 0)						\
		{									\
			ctnr_panic("WINDOW$ thread error !\n");				\
		}

/*-------------------------------------------------------------------------*/

typedef struct my_mutex_s
{
	HANDLE mutex;

} my_mutex_t;

/*-------------------------------------------------------------------------*/

void ctnr_mutex_allocate(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	object->mutex = CreateMutex(NULL, FALSE, NULL);

	if(object->mutex == NULL)
	{
		ctnr_panic("WINDOW$ thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mutex_deallocate(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	if(CloseHandle(object->mutex) == FALSE)
	{
		ctnr_panic("WINDOW$ thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mutex_lock(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	EnterCriticalSection(object->mutex);
}

/*-------------------------------------------------------------------------*/

void ctnr_mutex_unlock(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	LeaveCriticalSection(object->mutex);
}

/*-------------------------------------------------------------------------*/
/* MCOND								   */
/*-------------------------------------------------------------------------*/

#define SIGNAL		0
#define BROADCAST	1

/*-------------------------------------------------------------------------*/

typedef struct my_cond_s
{
	int cnt;

	HANDLE mutex1;
	HANDLE mutex2;

	HANDLE events[2];

} my_cond_t;

/*-------------------------------------------------------------------------*/

void ctnr_mcond_allocate(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	object->cnt = 0;

	/**/

	object->mutex1 = CreateMutex(NULL, FALSE, NULL);
	object->mutex2 = CreateMutex(NULL, FALSE, NULL);

	object->events[SIGNAL] = CreateEvent(NULL, FALSE, FALSE, NULL);
	object->events[BROADCAST] = CreateEvent(NULL, TRUE, FALSE, NULL);

	if(object->mutex1 == 0
	   ||
	   object->mutex2 == 0
	   ||
	   object->events[SIGNAL] == 0
	   ||
	   object->events[BROADCAST] == 0
	 ) {
		ctnr_panic("WINDOW$ thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_deallocate(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(CloseHandle(object->events[BROADCAST]) == FALSE
	   ||
	   CloseHandle(object->events[SIGNAL]) == FALSE
	   ||
	   CloseHandle(object->mutex2) == FALSE
	   ||
	   CloseHandle(object->mutex1) == FALSE
	 ) {
		ctnr_panic("WINDOW$ thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_lock(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	EnterCriticalSection(object->mutex1);
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_unlock(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	LeaveCriticalSection(object->mutex1);
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_signal(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	EnterCriticalSection(object->mutex2);
	/**/	bool have_waiters = object->cnt > 0;
	LeaveCriticalSection(object->mutex2);

	if(have_waiters != false)
	{
		SetEvent(object->events[SIGNAL]);
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_broadcast(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	EnterCriticalSection(object->mutex2);
	/**/	bool have_waiters = object->cnt > 0;
	LeaveCriticalSection(object->mutex2);

	if(have_waiters != false)
	{
		SetEvent(object->events[BROADCAST]);
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_wait(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	EnterCriticalSection(object->mutex2);
	/**/	object->cnt++;
	LeaveCriticalSection(object->mutex2);

	EnterCriticalSection(object->mutex1);

	/**/	int result = WaitForMultipleObjects(2, object->events, FALSE, INFINITE);
	/**/
	/**/	EnterCriticalSection(object->mutex2);
	/**/	/**/	object->cnt--; bool last_waiter = (result == WAIT_OBJECT_0 + BROADCAST) && (object->cnt == 0);
	/**/	LeaveCriticalSection(object->mutex2);
	/**/
	/**/	if(last_waiter != false)
	/**/	{
	/**/		ResetEvent(object->events[BROADCAST]);
	/**/	}

	LeaveCriticalSection(object->mutex1);
}

/*-------------------------------------------------------------------------*/

