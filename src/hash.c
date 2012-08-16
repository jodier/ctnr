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

unsigned ctnr_hash(const char *s)
{
	return ctnr_murmur32(ctnr_cast(BUFF_t, s), strlen(s) * sizeof(char), 0xAABBCCDD);
}

/*-------------------------------------------------------------------------*/

#define __get_LIST(list, index) *ctnr_cast(void **, &(list)->dict[index])

/*-------------------------------------------------------------------------*/

#define __get_name(list, item) *ctnr_cast(const char **, &ctnr_cast(uint8_t *, item)[(list)->offsets.name])
#define __get_prev(list, item) *ctnr_cast(/* */ void **, &ctnr_cast(uint8_t *, item)[(list)->offsets.prev])
#define __get_next(list, item) *ctnr_cast(/* */ void **, &ctnr_cast(uint8_t *, item)[(list)->offsets.next])

/*-------------------------------------------------------------------------*/

void __ctnr_hash_init(ctnr_hash_t *list, int offset_name, int offset_prev, int offset_next)
{
	list->offsets.name = offset_name;
	list->offsets.prev = offset_prev;
	list->offsets.next = offset_next;

	memset(list->dict, 0x00, CTNR_HASH_MAX * sizeof(void *));
}

/*-------------------------------------------------------------------------*/

void ctnr_hash_add(ctnr_hash_t *list, void *item)
{
	int index = ctnr_hash(__get_name(list, item)) % CTNR_HASH_MAX;

	void *LIST = __get_LIST(list, index);

	if(LIST != NULL)
	{
		__get_next(list, __get_prev(list, LIST)) = item;
		__get_prev(list, item) = __get_prev(list, LIST);
		__get_prev(list, LIST) = item;
		__get_next(list, item) = LIST;
	}
	else
	{
		__get_LIST(list, index) =  __get_prev(list, item) = __get_next(list, item) = item;
	}
}

/*-------------------------------------------------------------------------*/

void ctnr_hash_del(ctnr_hash_t *list, void *item)
{
	int index = ctnr_hash(__get_name(list, item)) % CTNR_HASH_MAX;

	/**/

	if(__get_prev(list, item) == item
	   &&
	   __get_next(list, item) == item
	 ) {
		__get_LIST(list, index) = NULL;
	}
	else
	{
		__get_next(list, __get_prev(list, item)) = __get_next(list, item);
		__get_prev(list, __get_next(list, item)) = __get_prev(list, item);

		if(__get_LIST(list, index) == item)
		{
			__get_LIST(list, index) = __get_next(list, item);
		}
	}

	__get_prev(list, item) = __get_next(list, item) = NULL;
}

/*-------------------------------------------------------------------------*/

bool ctnr_hash_lookup_item_by_hash(ctnr_hash_t *list, void *item, unsigned hash)
{
	int index = /*-----*/(hash) % CTNR_HASH_MAX;

	void *LIST = __get_LIST(list, index);

	if(LIST != NULL)
	{
		int nr;
		void *iter;

		for(iter = LIST, nr = 0; (nr == 0) || (iter != LIST); iter = __get_next(list, iter), nr++)
		{
			if(ctnr_hash(__get_name(list, iter)) == hash)
			{
				if(item != NULL)
				{
					*ctnr_cast(void **, item) = iter;
				}

				return true;
			}
		}
	}

	return false;
}

/*-------------------------------------------------------------------------*/

bool ctnr_hash_lookup_item_by_name(ctnr_hash_t *list, void *item, const char *name)
{
	int index = ctnr_hash(name) % CTNR_HASH_MAX;

	void *LIST = __get_LIST(list, index);

	if(LIST != NULL)
	{
		int nr;
		void *iter;

		unsigned hash = ctnr_hash(name);

		for(iter = LIST, nr = 0; (nr == 0) || (iter != LIST); iter = __get_next(list, iter), nr++)
		{
			if(ctnr_hash(__get_name(list, iter)) == hash)
			{
				if(item != NULL)
				{
					*ctnr_cast(void **, item) = iter;
				}

				return true;
			}
		}
	}

	return false;
}

/*-------------------------------------------------------------------------*/

void *ctnr_hash_lookup_entries_by_hash(ctnr_hash_t *list, unsigned hash)
{
	int index = /*-----*/(hash) % CTNR_HASH_MAX;

	return __get_LIST(list, index);
}

/*-------------------------------------------------------------------------*/

void *ctnr_hash_lookup_entries_by_name(ctnr_hash_t *list, const char *name)
{
	int index = ctnr_hash(name) % CTNR_HASH_MAX;

	return __get_LIST(list, index);
}

/*-------------------------------------------------------------------------*/

