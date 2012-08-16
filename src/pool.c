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

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

#define LOCK(pool)
#define UNLOCK(pool)

/*-------------------------------------------------------------------------*/

typedef struct ctnr_pool_item_s
{
	struct ctnr_pool_item_s *prev, *next;

} ctnr_pool_item_t;

/*-------------------------------------------------------------------------*/

buff_t __ctnr_pool_malloc(ctnr_pool_t *pool, size_t size, const char *file, int line)
{
	if(size == 0x00)
	{
		return NULL;
	}

	/**/

	ctnr_pool_item_t *item = ctnr_cast(ctnr_pool_item_t *, __ctnr_memory_malloc(sizeof(ctnr_pool_item_t) + size, file, line));

	LOCK(pool);

	/**/	ctnr_list_append(pool->list, item);
	/**/
	/**/	pool->size += size;

	UNLOCK(pool);

	return item + 1;
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_pool_malloc_zero(ctnr_pool_t *pool, size_t size, const char *file, int line)
{
	if(size == 0x00)
	{
		return NULL;
	}

	/**/

	ctnr_pool_item_t *item = ctnr_cast(ctnr_pool_item_t *, __ctnr_memory_malloc_zero(sizeof(ctnr_pool_item_t) + size, file, line));

	LOCK(pool);

	/**/	ctnr_list_append(pool->list, item);
	/**/
	/**/	pool->size += size;

	UNLOCK(pool);

	return item + 1;
}

/*-------------------------------------------------------------------------*/

static __inline__ buff_t __pool_realloc(ctnr_pool_t *pool, buff_t buff, size_t size, const char *file, int line)
{
	ctnr_pool_item_t *item = ctnr_cast(ctnr_pool_item_t *, buff) - 1;

	LOCK(pool);

	/**/	ctnr_list_remove(pool->list, item);
	/**/
	/**/	pool->size -= ctnr_memory_get_size(item);
	/**/
	/**/	item = ctnr_cast(ctnr_pool_item_t *, __ctnr_memory_realloc(item, sizeof(ctnr_pool_item_t) + size, file, line));
	/**/
	/**/	pool->size += ctnr_memory_get_size(item);
	/**/
	/**/	ctnr_list_append(pool->list, item);

	UNLOCK(pool);

	return item + 1;
}

/*-------------------------------------------------------------------------*/

static __inline__ buff_t __pool_realloc_zero(ctnr_pool_t *pool, buff_t buff, size_t size, const char *file, int line)
{
	ctnr_pool_item_t *item = ctnr_cast(ctnr_pool_item_t *, buff) - 1;

	LOCK(pool);

	/**/	ctnr_list_remove(pool->list, item);
	/**/
	/**/	pool->size -= ctnr_memory_get_size(item);
	/**/
	/**/	item = ctnr_cast(ctnr_pool_item_t *, __ctnr_memory_realloc_zero(item, sizeof(ctnr_pool_item_t) + size, file, line));
	/**/
	/**/	pool->size += ctnr_memory_get_size(item);
	/**/
	/**/	ctnr_list_append(pool->list, item);

	UNLOCK(pool);

	return item + 1;
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_pool_realloc(ctnr_pool_t *pool, buff_t buff, size_t size, const char *file, int line)
{
	buff_t result;

	if(buff == NULL)
	{
		if(size == 0x00)
		{
/*			__ctnr_pool_free(pool, buff, file, line);
 */
			result = NULL;
		}
		else {
			result = __ctnr_pool_malloc(pool, size, file, line);
		}
	}
	else
	{
		if(size == 0x00)
		{
			__ctnr_pool_free(pool, buff, file, line);

			result = NULL;
		}
		else {
			result = __pool_realloc(pool, buff, size, file, line);
		}
	}

	return result;
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_pool_realloc_zero(ctnr_pool_t *pool, buff_t buff, size_t size, const char *file, int line)
{
	buff_t result;

	if(buff == NULL)
	{
		if(size == 0x00)
		{
/*			__ctnr_pool_free_zero(pool, buff, file, line);
 */
			result = NULL;
		}
		else {
			result = __ctnr_pool_malloc_zero(pool, size, file, line);
		}
	}
	else
	{
		if(size == 0x00)
		{
			__ctnr_pool_free_zero(pool, buff, file, line);

			result = NULL;
		}
		else {
			result = __pool_realloc_zero(pool, buff, size, file, line);
		}
	}

	return result;
}

/*-------------------------------------------------------------------------*/

size_t __ctnr_pool_free(ctnr_pool_t *pool, buff_t buff, const char *file, int line)
{
	if(buff == NULL)
	{
		return 0x00;
	}

	/**/

	size_t size;

	ctnr_pool_item_t *item = ctnr_cast(ctnr_pool_item_t *, buff) - 1;

	LOCK(pool);

	/**/	ctnr_list_remove(pool->list, item);
	/**/
	/**/	size = __ctnr_memory_free(item, file, line) - sizeof(ctnr_pool_item_t);
	/**/
	/**/	pool->size -= size;

	UNLOCK(pool);

	return size;
}

/*-------------------------------------------------------------------------*/

size_t __ctnr_pool_free_zero(ctnr_pool_t *pool, buff_t buff, const char *file, int line)
{
	if(buff == NULL)
	{
		return 0x00;
	}

	/**/

	size_t size;

	ctnr_pool_item_t *item = ctnr_cast(ctnr_pool_item_t *, buff) - 1;

	LOCK(pool);

	/**/	ctnr_list_remove(pool->list, item);
	/**/
	/**/	size = __ctnr_memory_free_zero(item, file, line) - sizeof(ctnr_pool_item_t);
	/**/
	/**/	pool->size -= size;

	UNLOCK(pool);

	return size;
}

/*-------------------------------------------------------------------------*/

size_t __ctnr_pool_flush(ctnr_pool_t *pool, const char *file, int line)
{
	size_t result;

	ctnr_pool_item_t *item;

	LOCK(pool);

	/**/	result = pool->size;
	/**/
	/**/	while((item = ctnr_list_get_head(pool->list)) != NULL)
	/**/	{
	/**/		ctnr_list_remove(pool->list, item);
	/**/
	/**/		pool->size -= __ctnr_memory_free(item, file, line) - sizeof(ctnr_pool_item_t);
	/**/	}
	/**/
	/**/	if(pool->size != 0)
	/**/	{
	/**/		ctnr_panic("ctnr_pool_flush() - runtime error !\n");
	/**/	}

	UNLOCK(pool);

	return result;
}

/*-------------------------------------------------------------------------*/

size_t __ctnr_pool_flush_zero(ctnr_pool_t *pool, const char *file, int line)
{
	size_t result;

	ctnr_pool_item_t *item;

	LOCK(pool);

	/**/	result = pool->size;
	/**/
	/**/	while((item = ctnr_list_get_head(pool->list)) != NULL)
	/**/	{
	/**/		ctnr_list_remove(pool->list, item);
	/**/
	/**/		pool->size -= __ctnr_memory_free_zero(item, file, line) - sizeof(ctnr_pool_item_t);
	/**/	}
	/**/
	/**/	if(pool->size != 0)
	/**/	{
	/**/		ctnr_panic("ctnr_pool_flush_zero() - runtime error !\n");
	/**/	}

	UNLOCK(pool);

	return result;
}

/*-------------------------------------------------------------------------*/

