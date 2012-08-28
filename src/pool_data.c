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

#define LOCK(pool)
#define UNLOCK(pool)

/*-------------------------------------------------------------------------*/

typedef struct ctnr_data_item_s
{
	buff_t buff;
	size_t size;
	enum_t type;

	struct ctnr_data_item_s *prev, *next;

} ctnr_data_item_t;

/*-------------------------------------------------------------------------*/

bool ctnr_pool_data_push(ctnr_pool_t *pool, ctnr_data_t *data, BUFF_t buff, size_t size, enum_t type)
{
	ctnr_data_item_t *item = ctnr_cast(ctnr_data_item_t *, ctnr_pool_malloc(pool, sizeof(ctnr_data_item_t) + size));

	item->buff = item + 1;
	item->size = size;
	item->type = type;

	memcpy(item->buff, buff, size);

	LOCK(data);

	/**/	ctnr_list_append_tail(data->list, item);
	/**/
	/**/	data->size += size;

	UNLOCK(data);

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_pool_data_pop_head(ctnr_pool_t *pool, ctnr_data_t *data, buff_t buff, size_t *size, enum_t *type)
{
	ctnr_data_item_t *item;

	LOCK(data);

	/**/	item = ctnr_list_get_head(data->list);
	/**/
	/**/	if(item == NULL)
	/**/	{
	/**/		UNLOCK(data);
	/**/
	/**/		return false;
	/**/	}
	/**/
	/**/	data->size -= item->size;
	/**/
	/**/	ctnr_list_remove(data->list, item);

	UNLOCK(data);

	if(buff) {
		memcpy(buff, item->buff, item->size);
	}

	if(size) {
		*size = item->size;
	}

	if(type) {
		*type = item->type;
	}

	if(item->size != ctnr_pool_free(pool, item) - sizeof(ctnr_data_item_t))
	{
		ctnr_panic("ctnr_pool_data_pop_head() - runtime error !\n");
	}

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_pool_data_pop_tail(ctnr_pool_t *pool, ctnr_data_t *data, buff_t buff, size_t *size, enum_t *type)
{
	ctnr_data_item_t *item;

	LOCK(data);

	/**/	item = ctnr_list_get_tail(data->list);
	/**/
	/**/	if(item == NULL)
	/**/	{
	/**/		UNLOCK(data);
	/**/
	/**/		return false;
	/**/	}
	/**/
	/**/	data->size -= item->size;
	/**/
	/**/	ctnr_list_remove(data->list, item);

	UNLOCK(data);

	if(buff) {
		memcpy(buff, item->buff, item->size);
	}

	if(size) {
		*size = item->size;
	}

	if(type) {
		*type = item->type;
	}

	if(item->size != ctnr_pool_free(pool, item) - sizeof(ctnr_data_item_t))
	{
		ctnr_panic("ctnr_pool_data_pop_head() - runtime error !\n");
	}

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_pool_data_get_head(ctnr_pool_t *pool, ctnr_data_t *data, buff_t buff, size_t *size, enum_t *type)
{
	ctnr_data_item_t *item;

	LOCK(data);

	/**/	item = ctnr_list_get_head(data->list);
	/**/
	/**/	if(item == NULL)
	/**/	{
	/**/		UNLOCK(data);
	/**/
	/**/		return false;
	/**/	}
	/**/
	/**/	if(buff) {
	/**/		memcpy(buff, item->buff, item->size);
	/**/	}
	/**/
	/**/	if(size) {
	/**/		*size = item->size;
	/**/	}
	/**/
	/**/	if(type) {
	/**/		*type = item->type;
	/**/	}

	UNLOCK(data);

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_pool_data_get_tail(ctnr_pool_t *pool, ctnr_data_t *data, buff_t buff, size_t *size, enum_t *type)
{
	ctnr_data_item_t *item;

	LOCK(data);

	/**/	item = ctnr_list_get_tail(data->list);
	/**/
	/**/	if(item == NULL)
	/**/	{
	/**/		UNLOCK(data);
	/**/
	/**/		return false;
	/**/	}
	/**/
	/**/	if(buff) {
	/**/		memcpy(buff, item->buff, item->size);
	/**/	}
	/**/
	/**/	if(size) {
	/**/		*size = item->size;
	/**/	}
	/**/
	/**/	if(type) {
	/**/		*type = item->type;
	/**/	}

	UNLOCK(data);

	return true;
}

/*-------------------------------------------------------------------------*/

void ctnr_pool_data_append(ctnr_pool_t *pool, ctnr_data_t *data1, ctnr_data_t *data2)
{
	ctnr_data_item_t *item;

	LOCK(data1);
	LOCK(data2);

	/**/	while((item = ctnr_list_get_head(data2->list)) != NULL)
	/**/	{
	/**/		ctnr_list_remove(data2->list, item);
	/**/
	/**/		data2->size -= item->size;
	/**/		data1->size += item->size;
	/**/
	/**/		ctnr_list_append(data1->list, item);
	/**/	}

	UNLOCK(data2);
	UNLOCK(data1);
}

/*-------------------------------------------------------------------------*/

size_t ctnr_pool_data_clear(ctnr_pool_t *pool, ctnr_data_t *data)
{
	size_t result;

	ctnr_data_item_t *item;

	LOCK(data);

	/**/	result = data->size;
	/**/
	/**/	while((item = ctnr_list_get_head(data->list)) != NULL)
	/**/	{
	/**/		ctnr_list_remove(data->list, item);
	/**/
	/**/		data->size -= ctnr_pool_free(pool, item) - sizeof(ctnr_data_item_t);
	/**/	}
	/**/
	/**/	if(data->size != 0)
	/**/	{
	/**/		ctnr_panic("ctnr_pool_data_clear() - runtime error !\n");
	/**/	}

	UNLOCK(data);

	return result;
}

/*-------------------------------------------------------------------------*/

