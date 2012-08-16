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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

#define __memory_get_size(buff) (ctnr_cast(ctnr_memory_leak_t *, buff) - 1)->size
#define __memory_get_file(buff) (ctnr_cast(ctnr_memory_leak_t *, buff) - 1)->file
#define __memory_get_line(buff) (ctnr_cast(ctnr_memory_leak_t *, buff) - 1)->line

/*-------------------------------------------------------------------------*/

static ctnr_mutex_t mutex;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_memory_leak_s
{
	size_t size;
	const char *file;
	int line;

	struct ctnr_memory_leak_s *prev, *next;

} ctnr_memory_leak_t;

/*-------------------------------------------------------------------------*/

static ctnr_memory_leak_t *leak_list = NULL;

/*-------------------------------------------------------------------------*/

#define CHECK(leak) \
		if((leak) == NULL)				\
		{						\
			ctnr_panic("Out of memory !\n");	\
		}						\

/*-------------------------------------------------------------------------*/

buff_t __ctnr_memory_malloc(size_t size, const char *file, int line)
{
	if(size == 0x00)
	{
		return NULL;
	}

	/**/

	ctnr_memory_leak_t *leak;

	CHECK(leak = ctnr_cast(ctnr_memory_leak_t *, malloc(sizeof(ctnr_memory_leak_t) + size)));

	leak->size = size;
	leak->file = file;
	leak->line = line;

	ctnr_mutex_lock(&mutex);
	/**/	ctnr_list_append(leak_list, leak);
	ctnr_mutex_unlock(&mutex);

	return leak + 1;
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_memory_malloc_zero(size_t size, const char *file, int line)
{
	if(size == 0x00)
	{
		return NULL;
	}

	/**/

	ctnr_memory_leak_t *leak;

	CHECK(leak = ctnr_cast(ctnr_memory_leak_t *, malloc(sizeof(ctnr_memory_leak_t) + size)));

	leak->size = size;
	leak->file = file;
	leak->line = line;

	ctnr_mutex_lock(&mutex);
	/**/	ctnr_list_append(leak_list, leak);
	ctnr_mutex_unlock(&mutex);

	return memset(leak + 1, 0x00, size);
}

/*-------------------------------------------------------------------------*/

static __inline__ buff_t __memory_realloc(buff_t buff, size_t size, const char *file, int line)
{
	ctnr_memory_leak_t *leak = ctnr_cast(ctnr_memory_leak_t *, buff) - 1;

	ctnr_mutex_lock(&mutex);

	/**/	ctnr_list_remove(leak_list, leak);
	/**/
	/**/	CHECK(leak = ctnr_cast(ctnr_memory_leak_t *, realloc(leak, sizeof(ctnr_memory_leak_t) + size)));
	/**/
	/**/	leak->size = size;
	/**/	leak->file = file;
	/**/	leak->line = line;
	/**/
	/**/	ctnr_list_append(leak_list, leak);

	ctnr_mutex_unlock(&mutex);

	return leak + 1;
}

/*-------------------------------------------------------------------------*/

static __inline__ buff_t __memory_realloc_zero(buff_t buff, size_t size, const char *file, int line)
{
	/*-----------------------------------------------------------------*/

	size_t ezis = __memory_get_size(buff);

	if(ezis > size) memset(ctnr_cast(uint8_t *, buff) + size, 0x00, ezis - size);

	/*-----------------------------------------------------------------*/

	buff_t ffub = __memory_realloc(buff, size, file, line);

	if(size > ezis) memset(ctnr_cast(uint8_t *, ffub) + ezis, 0x00, size - ezis);

	/*-----------------------------------------------------------------*/

	return ffub;
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_memory_realloc(buff_t buff, size_t size, const char *file, int line)
{
	buff_t result;

	if(buff == NULL)
	{
		if(size == 0x00)
		{
/*			__ctnr_memory_free(buff, file, line);
 */
			result = NULL;
		}
		else {
			result = __ctnr_memory_malloc(size, file, line);
		}
	}
	else
	{
		if(size == 0x00)
		{
			__ctnr_memory_free(buff, file, line);

			result = NULL;
		}
		else {
			result = __memory_realloc(buff, size, file, line);
		}
	}

	return result;
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_memory_realloc_zero(buff_t buff, size_t size, const char *file, int line)
{
	buff_t result;

	if(buff == NULL)
	{
		if(size == 0x00)
		{
/*			__ctnr_memory_free_zero(buff, file, line);
 */
			result = NULL;
		}
		else {
			result = __ctnr_memory_malloc_zero(size, file, line);
		}
	}
	else
	{
		if(size == 0x00)
		{
			__ctnr_memory_free_zero(buff, file, line);

			result = NULL;
		}
		else {
			result = __memory_realloc_zero(buff, size, file, line);
		}
	}

	return result;
}

/*-------------------------------------------------------------------------*/

size_t __ctnr_memory_free(buff_t buff, const char *file, int line)
{
	if(buff == NULL)
	{
		return 0x00;
	}

	/**/

	size_t result;

	ctnr_memory_leak_t *leak = ctnr_cast(ctnr_memory_leak_t *, buff) - 1;

	ctnr_mutex_lock(&mutex);
	/**/	ctnr_list_remove(leak_list, leak);
	ctnr_mutex_unlock(&mutex);

	result = leak->size;

	free(leak);

	return result;
}

/*-------------------------------------------------------------------------*/

size_t __ctnr_memory_free_zero(buff_t buff, const char *file, int line)
{
	if(buff == NULL)
	{
		return 0x00;
	}

	/**/

	size_t result;

	ctnr_memory_leak_t *leak = ctnr_cast(ctnr_memory_leak_t *, buff) - 1;

	ctnr_mutex_lock(&mutex);
	/**/	ctnr_list_remove(leak_list, leak);
	ctnr_mutex_unlock(&mutex);

	memset(buff, 0x00, result = leak->size);

	free(leak);

	return result;
}

/*-------------------------------------------------------------------------*/

size_t ctnr_memory_get_size(buff_t buff)
{
	return (buff != NULL) ? __memory_get_size(buff) : 0x00;
}

const char *ctnr_memory_get_file(buff_t buff)
{
	return (buff != NULL) ? __memory_get_file(buff) : NULL;
}

int ctnr_memory_get_line(buff_t buff)
{
	return (buff != NULL) ? __memory_get_line(buff) : 0x00;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

DLL_HIDDEN void __ctnr_memory_ctor(void)
{
	ctnr_mutex_allocate(&mutex);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

DLL_HIDDEN void __ctnr_memory_dtor(void)
{
	if(leak_list != NULL)
	{
		ctnr_memory_leak_t *leak;

		fprintf(stderr, "Leak list:\n");

		while((leak = ctnr_list_get_head(leak_list)) != NULL)
		{
			fprintf(stderr, " -> size = '%lu byte(s)', file = '%s', line = '%d'\n", leak->size, leak->file, leak->line);

			ctnr_list_remove(leak_list, leak);

			free(leak);
		}
	}

	/**/

	ctnr_mutex_deallocate(&mutex);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

