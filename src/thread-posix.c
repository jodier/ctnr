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

#include <pthread.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/
/* THREAD								   */
/*-------------------------------------------------------------------------*/

typedef struct my_thread_s
{
	pthread_t thread;

} my_thread_t;

/*-------------------------------------------------------------------------*/

void ctnr_thread_create(ctnr_thread_t *thread, void *(* func)(void *), void *arg)
{
	my_thread_t *object = ctnr_cast(my_thread_t *, thread->backend);

	/**/

	if(pthread_create(&object->thread, NULL, func, arg) != 0)
	{
		ctnr_panic("ctnr_thread_create: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void *ctnr_thread_waitfor(ctnr_thread_t *thread)
{
	my_thread_t *object = ctnr_cast(my_thread_t *, thread->backend);

	/**/

	void *result;

	if(pthread_join(object->thread, &result) != 0)
	{
		ctnr_panic("ctnr_thread_waitfor: POSIX thread error !\n");
	}

	return result;
}

/*-------------------------------------------------------------------------*/

void ctnr_thread_exit(void *ret)
{
	pthread_exit(ret);

	ctnr_panic("ctnr_thread_exit: POSIX thread error !\n");
}

/*-------------------------------------------------------------------------*/
/* MUTEX								   */
/*-------------------------------------------------------------------------*/

typedef struct my_mutex_s
{
	pthread_mutex_t mutex;

} my_mutex_t;

/*-------------------------------------------------------------------------*/

void ctnr_mutex_allocate(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	if(pthread_mutex_init(&object->mutex, NULL) != 0)
	{
		ctnr_panic("ctnr_mutex_allocate: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mutex_deallocate(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	if(pthread_mutex_destroy(&object->mutex) != 0)
	{
		ctnr_panic("ctnr_mutex_deallocate: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mutex_lock(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	if(pthread_mutex_lock(&object->mutex) != 0)
	{
		ctnr_panic("ctnr_mutex_lock: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mutex_unlock(ctnr_mutex_t *mutex)
{
	my_mutex_t *object = ctnr_cast(my_mutex_t *, mutex->backend);

	/**/

	if(pthread_mutex_unlock(&object->mutex) != 0)
	{
		ctnr_panic("ctnr_mutex_unlock: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/
/* MCOND								   */
/*-------------------------------------------------------------------------*/

typedef struct my_cond_s
{
	pthread_mutex_t *mutex;

	pthread_cond_t *cond;

} my_cond_t;

/*-------------------------------------------------------------------------*/

void ctnr_mcond_allocate(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_mutex_init(object->mutex, NULL) != 0
	   ||
	   pthread_cond_init(object->cond, NULL) != 0
	 ) {
		ctnr_panic("ctnr_mcond_allocate: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_deallocate(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_cond_destroy(object->cond) != 0
	   ||
	   pthread_mutex_destroy(object->mutex) != 0
	 ) {
		ctnr_panic("ctnr_mcond_deallocate: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_lock(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_mutex_lock(object->mutex) != 0)
	{
		ctnr_panic("ctnr_mcond_lock: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_unlock(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_mutex_unlock(object->mutex) != 0)
	{
		ctnr_panic("ctnr_mcond_unlock: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_signal(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_cond_signal(object->cond) != 0)
	{
		ctnr_panic("ctnr_mcond_signal: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_broadcast(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_cond_broadcast(object->cond) != 0)
	{
		ctnr_panic("ctnr_mcond_broadcast: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_mcond_wait(ctnr_mcond_t *mcond)
{
	my_cond_t *object = ctnr_cast(my_cond_t *, mcond->backend);

	/**/

	if(pthread_cond_wait(object->cond, object->mutex) != 0)
	{
		ctnr_panic("ctnr_mcond_wait: POSIX thread error !\n");
	}
}

/*-------------------------------------------------------------------------*/

