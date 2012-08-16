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
#include <stdlib.h>
#include <string.h>

#include <objc/runtime.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

static ctnr_mutex_t mutex;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_objc_descr_s
{
	ID object_ID;
	id object_id;

	struct ctnr_objc_descr_s *prev, *next;

} ctnr_objc_descr_t;

/*-------------------------------------------------------------------------*/

static ctnr_hash_t object_descr1_list;
static ctnr_hash_t object_descr2_list;

/*-------------------------------------------------------------------------*/

static void *__malloc(size_t size)
{
	void *result = malloc(size);

	if(result == NULL)
	{
		ctnr_panic("Out of memory !\n");
	}

	return memset(result, 0x00, size);
}

/*-------------------------------------------------------------------------*/

DLL_PUBLIC id ctnr_objc_alloc_object(Class class)
{
	const char *name = \
		class_getName(class);

	ctnr_objc_descr_t *descr = ctnr_cast(ctnr_objc_descr_t *, __malloc(sizeof(ctnr_objc_descr_t) + class_getInstanceSize(class) + strlen(name) + 16));

	descr->object_ID = ctnr_cast(ID, ctnr_cast(uint8_t *, descr) + sizeof(ctnr_objc_descr_t) + class_getInstanceSize(class));
	descr->object_id = ctnr_cast(id, ctnr_cast(uint8_t *, descr) + sizeof(ctnr_objc_descr_t) + 0x00000000000000000000000000);

#ifdef __NEXT_RUNTIME__
	descr->object_id->     isa      = class;
#else
	descr->object_id->class_pointer = class;
#endif
	/**/

	static int __nr = 0;

	/**/

	ctnr_mutex_lock(&mutex);

	/**/	if(ctnr_hash_lookup_item_by_name(&object_descr2_list, NULL, name) != false
	/**/	   ||
	/**/	   ctnr_hash_lookup_item_by_name(&object_descr1_list, NULL, name) != false
	/**/	 ) {
	/**/		ctnr_mutex_unlock(&mutex);
	/**/
	/**/		ctnr_panic("Only one instanciation of '%s' allowed !\n", name);
	/**/	}
	/**/
	/**/	/**/
	/**/
	/**/	if(CTNR_OBJC_IS_SINGLETON(class) == false)
	/**/	{
	/**/		sprintf(ctnr_cast(char *, descr->object_ID), "%s%c%d", name, '@', __nr);
	/**/
	/**/		ctnr_hash_add(&object_descr1_list, descr);
	/**/	}
	/**/	else
	/**/	{
	/**/		sprintf(ctnr_cast(char *, descr->object_ID), "%s%c%d", name, 0x0, __nr);
	/**/
	/**/		ctnr_hash_add(&object_descr2_list, descr);
	/**/	}
	/**/
	/**/	__nr++;

	ctnr_mutex_unlock(&mutex);

	return descr->object_id;
}

/*-------------------------------------------------------------------------*/

DLL_PUBLIC id ctnr_objc_dealloc_object(id object)
{
	ctnr_objc_descr_t *descr = ctnr_cast(ctnr_objc_descr_t *, object) - 1;

	ctnr_mutex_lock(&mutex);

#ifdef __NEXT_RUNTIME__
	/**/	if(CTNR_OBJC_IS_SINGLETON(object->     isa     ) == false)
#else
	/**/	if(CTNR_OBJC_IS_SINGLETON(object->class_pointer) == false)
#endif
	/**/	{
	/**/		ctnr_hash_del(&object_descr1_list, descr);
	/**/	}
	/**/	else
	/**/	{
	/**/		ctnr_hash_del(&object_descr2_list, descr);
	/**/	}

	ctnr_mutex_unlock(&mutex);

	free(descr);

	return nil;
}

/*-------------------------------------------------------------------------*/
/* TObject								   */
/*-------------------------------------------------------------------------*/

@implementation TObject

	/*-----------------------------------------------------------------*/
	/* CONSTRUCTOR & DESTRUCTOR					   */
	/*-----------------------------------------------------------------*/

	+ (void) initialize {}

	/*-----------------------------------------------------------------*/

	- (id) init
	{
		self->m_refcnt = 1;

		return self;
	}

	/*-----------------------------------------------------------------*/

	+ (id) alloc
	{
		return ctnr_objc_alloc_object(self);
	}

	/*-----------------------------------------------------------------*/

	- (id) dealloc
	{
		/*---------------------------------------------------------*/
		/* EXTERNAL OBJECTS					   */
		/*---------------------------------------------------------*/

		[self poolFlush];

		if(self->m_parent != nil)
		{
			[self->m_parent poolDel: self];
		}

		/*---------------------------------------------------------*/
		/* INTERNAL OBJECTS					   */
		/*---------------------------------------------------------*/

		ctnr_pool_flush(&self->m_pool);

		/*---------------------------------------------------------*/

		return ctnr_objc_dealloc_object(self);
	}

	/*-----------------------------------------------------------------*/

	- (id) free
	{
		printf("* LINUX *\n");

		return [self dealloc];
	}

	/*-----------------------------------------------------------------*/
	/* MEMORY							   */
	/*-----------------------------------------------------------------*/

	- (id) retain
	{
/*		@synchronized(self)
 */		{
			self->m_refcnt++;

			return /*-----------------------------------*/ self;
		}
	}

	/*-----------------------------------------------------------------*/

	- (id) release
	{
/*		@synchronized(self)
 */		{
			self->m_refcnt--;

			return (self->m_refcnt < 1) ? [self dealloc] : self;
		}
	}

	/*-----------------------------------------------------------------*/

	- (int) retainCount
	{
/*		@synchronized(self)
 */		{
			return self->m_refcnt;
		}
	}

	/*-----------------------------------------------------------------*/

	- (buff_t) memAlloc: (size_t) size
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_malloc(&self->m_pool, size);
		}
	}

	/*-----------------------------------------------------------------*/

	- (buff_t) memAllocZero: (size_t) size
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_malloc_zero(&self->m_pool, size);
		}
	}

	/*-----------------------------------------------------------------*/

	- (buff_t) memRealloc: (buff_t) buff: (size_t) size
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_realloc(&self->m_pool, buff, size);
		}
	}

	/*-----------------------------------------------------------------*/

	- (buff_t) memReallocZero: (buff_t) buff: (size_t) size
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_realloc_zero(&self->m_pool, buff, size);
		}
	}

	/*-----------------------------------------------------------------*/

	- (size_t) memFree: (buff_t) buff
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_free(&self->m_pool, buff);
		}
	}

	/*-----------------------------------------------------------------*/

	- (size_t) memFreeZero: (buff_t) buff
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_free_zero(&self->m_pool, buff);
		}
	}

	/*-----------------------------------------------------------------*/
	/* STRINGS							   */
	/*-----------------------------------------------------------------*/

	- (char *) strDup: (const char *) str
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_strdup(&self->m_pool, str);
		}
	}

	/*-----------------------------------------------------------------*/

	- (char *) strNDup: (const char *) str: (size_t) size
	{
/*		@synchronized(self)
 */		{
			return ctnr_pool_strndup(&self->m_pool, str, size);
		}
	}


	/*-----------------------------------------------------------------*/
	/* POOL								   */
	/*-----------------------------------------------------------------*/

	- (id) poolAdd: (TObject *) anObject
	{
/*		@synchronized(self)
 */		{
			if(anObject != nil)
			{
				if(anObject->m_parent != nil
				   ||
				   anObject->m_prev != nil
				   ||
				   anObject->m_next != nil
				 ) {
					ctnr_panic("Runtime error (pool corruption) !\n");
				}

				anObject->m_parent = self;

				ctnr_list_append_named(self->m_list, anObject, m_prev, m_next);
			}

			return anObject;
		}
	}

	/*-----------------------------------------------------------------*/

	- (id) poolDel: (TObject *) anObject
	{
/*		@synchronized(self)
 */		{
			if(anObject != nil)
			{
				if(anObject->m_parent == nil
				   ||
				   anObject->m_prev == nil
				   ||
				   anObject->m_next == nil
				 ) {
					ctnr_panic("Runtime error (pool corruption) !\n");
				}

				anObject->m_parent = nil;

				ctnr_list_remove_named(self->m_list, anObject, m_prev, m_next);
			}

			return anObject;
		}
	}

	/*-----------------------------------------------------------------*/

	- (void) poolFlush
	{
/*		@synchronized(self)
 */		{
			TObject *object;

			while((object = ctnr_list_get_head_named(self->m_list, m_prev, m_next)) != nil)
			{
				[[self poolDel: object] release];
			}
		}
	}

	/*-----------------------------------------------------------------*/
	/* INTROSPECTION						   */
	/*-----------------------------------------------------------------*/

	- (id) self
	{
		return self;
	}

	/*-----------------------------------------------------------------*/

	+ (Class) class
	{
		return ctnr_cast(Class, self);
	}

	/*-----------------------------------------------------------------*/

	- (Class) class
	{
		return ctnr_cast(Class, self->isa);
	}

	/*-----------------------------------------------------------------*/

	+ (Class) superclass
	{
		return class_getSuperclass(self);
	}

	/*-----------------------------------------------------------------*/

	- (Class) superclass
	{
		return class_getSuperclass(self->isa);
	}

	/*-----------------------------------------------------------------*/

	#define object_getUniqueName(object) (ctnr_cast(ctnr_objc_descr_t *, object) - 1)->object_ID

	/*-----------------------------------------------------------------*/

	- (const char *) className
	{
		return object_getClassName(self);
	}

	/*-----------------------------------------------------------------*/

	- (const char *) uniqueName
	{
		return object_getUniqueName(self);
	}

	/*-----------------------------------------------------------------*/

	+ (bool) isSubclassOfClass: (Class) aClass
	{
		Class class;

		for(class = ctnr_cast(Class, self); class != Nil; class = class_getSuperclass(class))
		{
			if(class == aClass)
			{
				return true;
			}
		}

		return false;
	}

	/*-----------------------------------------------------------------*/

	+ (bool) instancesRespondToSelector: (SEL) aSelector
	{
		return class_getInstanceMethod(ctnr_cast(Class, self), aSelector) != NULL;
	}

	/*-----------------------------------------------------------------*/

	- (bool) respondToSelector: (SEL) aSelector
	{
		return class_getInstanceMethod(ctnr_cast(Class, self->isa), aSelector) != NULL;
	}

	/*-----------------------------------------------------------------*/

	+ (IMP) instanceMethodForSelector: (SEL) aSelector
	{
		return method_getImplementation(
			class_getClassMethod(ctnr_cast(Class, self), aSelector)
		);
	}

	/*-----------------------------------------------------------------*/

	- (IMP) methodForSelector: (SEL) aSelector
	{
		return method_getImplementation(
			class_getInstanceMethod(ctnr_cast(Class, self->isa), aSelector)
		);
	}

	/*-----------------------------------------------------------------*/

	+ (TMethodSignature *) instanceMethodSignatureForSelector: (SEL) aSelector
	{
		Method method = class_getInstanceMethod(ctnr_cast(Class, self), aSelector);

		return method != NULL ? [TMethodSignature signatureWithObjCTypes: method_getTypeEncoding(method)] : nil;
	}

	/*-----------------------------------------------------------------*/

	- (TMethodSignature *) methodSignatureForSelector: (SEL) aSelector
	{
		Method method = class_getInstanceMethod(ctnr_cast(Class, self->isa), aSelector);

		return method != NULL ? [TMethodSignature signatureWithObjCTypes: method_getTypeEncoding(method)] : nil;
	}

	/*-----------------------------------------------------------------*/

@end

/*-------------------------------------------------------------------------*/
/* TSingleton								   */
/*-------------------------------------------------------------------------*/

@implementation TSingleton

@end

/*-------------------------------------------------------------------------*/
/* FACTORIES								   */
/*-------------------------------------------------------------------------*/

cl ctnr_objc_class(const char *name)
{
	return objc_lookUpClass(name);
}

/*-------------------------------------------------------------------------*/

id ctnr_objc_object(const char *name)
{
	cl class = objc_lookUpClass(name);

	return (class != Nil) ? [class alloc] : nil;
}

/*-------------------------------------------------------------------------*/

id ctnr_objc_singleton(const char *name)
{
	id result = nil;

	ctnr_objc_descr_t *descr;

	ctnr_mutex_lock(&mutex);

	/**/	/**/ if(ctnr_hash_lookup_item_by_name(&object_descr2_list, &descr, name) != false) {
	/**/		result = descr->object_id;
	/**/	}
	/**/	else if(ctnr_hash_lookup_item_by_name(&object_descr1_list, &descr, name) != false) {
	/**/		result = descr->object_id;
	/**/	}
	/**/	else
	/**/	{
	/**/		printf("No instance for '%s' !\n", name);
	/**/	}

	ctnr_mutex_unlock(&mutex);

	return result;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

DLL_HIDDEN void __ctnr_objc_ctor(void)
{
	ctnr_mutex_allocate(&mutex);

	/**/

	ctnr_hash_init(&object_descr1_list, ctnr_objc_descr_t, object_ID);
	ctnr_hash_init(&object_descr2_list, ctnr_objc_descr_t, object_ID);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

DLL_HIDDEN void __ctnr_objc_dtor(void)
{
	int i;

	ctnr_objc_descr_t *list;
	ctnr_objc_descr_t *item;
 
	for(i = 0; i < CTNR_HASH_MAX; i++)
	{
		list = object_descr1_list.dict[i];

		while((item = ctnr_list_get_head(list)) != NULL)
		{
			printf("Warning: auto-deleting %s, retainCount: %d\n", item->object_ID, [item->object_id retainCount]);

			ctnr_list_remove(list, item);

			free(item);
		}

		list = object_descr2_list.dict[i];

		while((item = ctnr_list_get_head(list)) != NULL)
		{
			printf("Warning: auto-deleting %s, retainCount: %d\n", item->object_ID, [item->object_id retainCount]);

			ctnr_list_remove(list, item);

			free(item);
		}
	}

	/**/

	ctnr_mutex_deallocate(&mutex);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

