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

#ifndef __CTNR_LIST_H
#define __CTNR_LIST_H

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

#define ctnr_list_init_named(list, prev, next)					\
	((list) = NULL)

/*-------------------------------------------------------------------------*/

#define ctnr_list_singleton_named(list, item, prev, next)			\
({										\
	(list) = (item)->prev = (item)->next = (item);				\
})

/*-------------------------------------------------------------------------*/

#define ctnr_list_is_empty_named(list, prev, next)				\
	((list) == NULL)

/*-------------------------------------------------------------------------*/

#define ctnr_list_is_singleton_named(list, prev, next)				\
	((list) != NULL && (list) == (list)->prev && (list) == (list)->next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_get_head_named(list, prev, next)				\
	((list) != NULL ? (list) /* */ : NULL)

/*-------------------------------------------------------------------------*/

#define ctnr_list_get_tail_named(list, prev, next)				\
	((list) != NULL ? (list)->prev : NULL)

/*-------------------------------------------------------------------------*/

#define ctnr_list_is_chained_named(item, prev, next)				\
	((item)->prev != NULL && (item)->next != NULL)

/*-------------------------------------------------------------------------*/

#define __list_insert_atleft_named(existing_item, item, prev, next)		\
{										\
	(existing_item)->prev->next = (item);					\
	(item)->prev = (existing_item)->prev;					\
	(existing_item)->prev = (item);						\
	(item)->next = (existing_item);						\
}

/*-------------------------------------------------------------------------*/

#define __list_insert_atright_named(existing_item, item, prev, next)		\
{										\
	(existing_item)->next->prev = (item);					\
	(item)->next = (existing_item)->next;					\
	(existing_item)->next = (item);						\
	(item)->prev = (existing_item);						\
}

/*-------------------------------------------------------------------------*/

#define ctnr_list_append_head_named(list, item, prev, next)			\
{										\
	if((list) != NULL)							\
	{									\
		__list_insert_atleft_named(list, item, prev, next);		\
										\
		(list) = (item);						\
	}									\
	else									\
	{									\
		ctnr_list_singleton_named(list, item, prev, next);		\
	}									\
}

/*-------------------------------------------------------------------------*/

#define ctnr_list_append_tail_named(list, item, prev, next)			\
{										\
	if((list) != NULL)							\
	{									\
		__list_insert_atleft_named(list, item, prev, next);		\
										\
/*		(list) = (list);						\
 */	}									\
	else									\
	{									\
		ctnr_list_singleton_named(list, item, prev, next);		\
	}									\
}

/*-------------------------------------------------------------------------*/

#define ctnr_list_insert_atleft_named(list, existing_item, item, prev, next)	\
{										\
	if((list) != NULL)							\
	{									\
		__list_insert_atleft_named(existing_item, item, prev, next);	\
										\
		if((list) == (existing_item))					\
		{								\
			(list) = (item);					\
		}								\
	}									\
	else									\
	{									\
		ctnr_list_singleton_named(list, item, prev, next);		\
	}									\
}

/*-------------------------------------------------------------------------*/

#define ctnr_list_insert_atright_named(list, existing_item, item, prev, next)	\
{										\
	if((list) != NULL)							\
	{									\
		__list_insert_atright_named(existing_item, item, prev, next);	\
										\
/*		if((list) == (existing_item))					\
 *		{								\
 *			(list) = (list);					\
 *		}								\
 */	}									\
	else									\
	{									\
		ctnr_list_singleton_named(list, item, prev, next);		\
	}									\
}

/*-------------------------------------------------------------------------*/

#define ctnr_list_remove_named(list, item, prev, next)				\
{										\
	if(((item)->prev == (item))						\
	   &&									\
	   ((item)->next == (item))						\
	 ) {									\
		(list) = NULL;							\
	}									\
	else									\
	{									\
		(item)->prev->next = (item)->next;				\
		(item)->next->prev = (item)->prev;				\
										\
		if((list) == (item))						\
		{								\
			(list) = (__typeof__(list)) (item)->next;		\
		}								\
	}									\
										\
	(item)->prev = (item)->next = NULL;					\
}

/*-------------------------------------------------------------------------*/

#define ctnr_list_pop_head_named(list, prev, next)				\
({										\
	__typeof__(list) result = ctnr_list_get_head_named(list, prev, next);	\
										\
	if(result != NULL)							\
	{									\
		ctnr_list_remove_named(list, result, prev, next);		\
	}									\
										\
	result;									\
})

/*-------------------------------------------------------------------------*/

#define ctnr_list_pop_tail_named(list, prev, next)				\
({										\
	__typeof__(list) result = ctnr_list_get_tail_named(list, prev, next);	\
										\
	if(result != NULL)							\
	{									\
		ctnr_list_remove_named(list, result, prev, next);		\
	}									\
										\
	result;									\
})

/*-------------------------------------------------------------------------*/

#define ctnr_list_foreach_forward_named(list, iterator, nr, prev, next)		\
	if((list) != NULL) for((iterator) = ctnr_list_get_head_named(list, prev, next), (nr) = 0; ((nr) == 0) || ((iterator) != ctnr_list_get_head_named(list, prev, next)); (iterator) = (iterator)->next, (nr)++)

#define ctnr_list_foreach_backward_named(list, iterator, nr, prev, next)	\
	if((list) != NULL) for((iterator) = ctnr_list_get_tail_named(list, prev, next), (nr) = 0; ((nr) == 0) || ((iterator) != ctnr_list_get_tail_named(list, prev, next)); (iterator) = (iterator)->prev, (nr)++)

/*-------------------------------------------------------------------------*/

#define CTNR_TMP2(x, y) \
		x##y

#define CTNR_TMP1(x, y) \
		CTNR_TMP2(x, y)

#define CTNR_SYMB(x) \
		CTNR_TMP1(x, __LINE__)

/*-------------------------------------------------------------------------*/

#define ctnr_hash_foreach_forward_named(list, iterator, nr, prev, next) \
		int CTNR_SYMB(__tmp_nr);					\
		__typeof__(iterator) CTNR_SYMB(__tmp_list);			\
										\
		for(CTNR_SYMB(__tmp_list) = (__typeof__(iterator)) (list).dict[(CTNR_SYMB(__tmp_nr)) = 0]; (CTNR_SYMB(__tmp_nr)) <= CTNR_HASH_MAX - 1; CTNR_SYMB(__tmp_list) = (__typeof__(iterator)) (list).dict[(CTNR_SYMB(__tmp_nr)) = (CTNR_SYMB(__tmp_nr)) + 1]) \
			ctnr_list_foreach_forward_named(CTNR_SYMB(__tmp_list), iterator, nr, prev, next)

#define ctnr_hash_foreach_backward_named(list, iterator, nr, prev, next) \
		int CTNR_SYMB(__tmp_nr);					\
		__typeof__(iterator) CTNR_SYMB(__tmp_list);			\
										\
		for(CTNR_SYMB(__tmp_list) = (__typeof__(iterator)) (list).dict[(CTNR_SYMB(__tmp_nr)) = CTNR_HASH_MAX - 1]; (CTNR_SYMB(__tmp_nr)) >= 0; CTNR_SYMB(__tmp_list) = (__typeof__(iterator)) (list).dict[(CTNR_SYMB(__tmp_nr)) = (CTNR_SYMB(__tmp_nr)) - 1]) \
			ctnr_list_foreach_backward_named(CTNR_SYMB(__tmp_list), iterator, nr, prev, next)

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

#define ctnr_list_init(list) \
		ctnr_list_init_named(list, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_is_empty(list) \
		ctnr_list_is_empty_named(list, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_singleton(list, item) \
		ctnr_list_singleton_named(list, item, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_is_singleton(list) \
		ctnr_list_is_singleton_named(list, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_get_head(list) \
		ctnr_list_get_head_named(list, prev, next)

#define ctnr_list_get_tail(list) \
		ctnr_list_get_tail_named(list, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_is_chained(item) \
		ctnr_list_is_chained_named(item, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_append_head(list, item) \
		ctnr_list_append_head_named(list, item, prev, next)

#define ctnr_list_append_tail(list, item) \
		ctnr_list_append_tail_named(list, item, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_insert_atleft(list, existing_item, item) \
		ctnr_list_insert_atleft_named(list, existing_item, item, prev, next)

#define ctnr_list_insert_atright(list, existing_item, item) \
		ctnr_list_insert_atright_named(list, existing_item, item, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_remove(list, item) \
		ctnr_list_remove_named(list, item, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_pop_head(list) \
		ctnr_list_pop_head_named(list, prev, next)

#define ctnr_list_pop_tail(list) \
		ctnr_list_pop_tail_named(list, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_list_foreach_forward(list, iterator, nr) \
		ctnr_list_foreach_forward_named(list, iterator, nr, prev, next)

#define ctnr_list_foreach_backward(list, iterator, nr) \
		ctnr_list_foreach_backward_named(list, iterator, nr, prev, next)

/*-------------------------------------------------------------------------*/

#define ctnr_hash_foreach_forward(list, iterator, nr) \
		ctnr_hash_foreach_forward_named(list, iterator, nr, prev, next)

#define ctnr_hash_foreach_backward(list, iterator, nr) \
		ctnr_hash_foreach_backward_named(list, iterator, nr, prev, next)

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

#define ctnr_list_append_named(list, item, prev, next) \
		ctnr_list_append_tail_named(list, item, prev, next)

#define ctnr_list_pop_named(list, prev, next) \
		ctnr_list_pop_tail_named(list, prev, next)

#define ctnr_list_foreach_named(list, iterator, nr, prev, next) \
		ctnr_list_foreach_forward_named(list, iterator, nr, prev, next)

#define ctnr_hash_foreach_named(list, iterator, nr, prev, next) \
		ctnr_hash_foreach_forward_named(list, iterator, nr, prev, next)

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

#define ctnr_list_append(list, item) \
		ctnr_list_append_tail(list, item)

#define ctnr_list_pop(list) \
		ctnr_list_pop_tail(list)

#define ctnr_list_foreach(list, iterator, nr) \
		ctnr_list_foreach_forward(list, iterator, nr)

#define ctnr_hash_foreach(list, iterator, nr) \
		ctnr_hash_foreach_forward(list, iterator, nr)

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

#endif /* __CTNR_LIST_H */

/*-------------------------------------------------------------------------*/

