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

#include <string.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

#define GET_ITEM(pool, vect, index) ctnr_cast(buff_t, ctnr_cast(uintptr_t, (vect)->buff) + (index) * (vect)->data_size)

/*-------------------------------------------------------------------------*/

#define INC(pool, vect) \
		if((vect)->size % (vect)->cache_size == 0)											\
		{																\
			(vect)->buff = ctnr_pool_realloc(pool, (vect)->buff, (vect)->data_size * ((vect)->size + 1 * (vect)->cache_size));	\
		}

#define DEC(pool, vect) \
		if((vect)->size % (vect)->cache_size == 0)											\
		{																\
			(vect)->buff = ctnr_pool_realloc(pool, (vect)->buff, (vect)->data_size * ((vect)->size - 0 * (vect)->cache_size));	\
		}

/*-------------------------------------------------------------------------*/

void ctnr_pool_vect_clear(ctnr_pool_t *pool, ctnr_vect_t *vect)
{
	ctnr_pool_free(pool, vect->buff);

	vect->buff = NULL;
	vect->size = 0x00;
}

/*-------------------------------------------------------------------------*/

bool ctnr_pool_vect_push_back(ctnr_pool_t *pool, ctnr_vect_t *vect, BUFF_t buff)
{
	if(buff == NULL
	   ||
	   vect->size >= 0x3FFFFFFF
	 ) {
		return false;
	}

	/**/

	INC(pool, vect);
	memcpy(GET_ITEM(pool, vect, vect->size), buff, vect->data_size);
	vect->size++;

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_pool_vect_pop_back(ctnr_pool_t *pool, ctnr_vect_t *vect, buff_t buff)
{
	if(buff == NULL
	   ||
	   vect->size <= 0x00000000
	 ) {
		return false;
	}

	/**/

	vect->size--;
	memcpy(buff, GET_ITEM(pool, vect, vect->size), vect->data_size);
	DEC(pool, vect);

	return true;
}

/*-------------------------------------------------------------------------*/

void ctnr_pool_vect_del(ctnr_pool_t *pool, ctnr_vect_t *vect, size_t index)
{
	if(index >= vect->size)
	{
		ctnr_panic("Index out of range !\n");
	}

	/**/

	vect->size--;
	memcpy(GET_ITEM(pool, vect, index + 0), GET_ITEM(pool, vect, index + 1), (vect->size - index) * vect->data_size);
	DEC(pool, vect);
}

/*-------------------------------------------------------------------------*/

buff_t __ctnr_pool_vect_at(ctnr_pool_t *pool, ctnr_vect_t *vect, size_t index)
{
	if(index >= vect->size)
	{
		ctnr_panic("Index out of range !\n");
	}

	/**/

	return GET_ITEM(pool, vect, index);
}

/*-------------------------------------------------------------------------*/

