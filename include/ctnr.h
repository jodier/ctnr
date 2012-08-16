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

#ifndef __CTNR_H
#define __CTNR_H

/*-------------------------------------------------------------------------*/

#if __GNUC__ < 4
  #error "LIBCTNR requires at least gcc 4.0.0"
#endif

/*-------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "ctnr_list.h"

/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
  #define CTNR_BEGIN_EXTERN_C	   extern "C" {
  #define CTNR_END_EXTERN_C	   }
#else
  #define CTNR_BEGIN_EXTERN_C	/* extern "C" { */
  #define CTNR_END_EXTERN_C	/* } */
#endif

/*-------------------------------------------------------------------------*/

#undef DLL_PUBLIC
#undef DLL_HIDDEN

/*-------------------------------------------------------------------------*/

#ifdef __IS_WIN
  #define DLL_PUBLIC
  #define DLL_HIDDEN
#else
  #define DLL_PUBLIC \
		__attribute__ ((visibility("default")))
  #define DLL_HIDDEN \
		__attribute__ ((visibility("hidden")))
#endif

/*-------------------------------------------------------------------------*/

#define __CTNR_CTOR \
		__attribute__ ((constructor))
#define __CTNR_DTOR \
		__attribute__ ((destructor))

/*-------------------------------------------------------------------------*/

#define ctnr_cast(x, y) ((x) (y))

/*-------------------------------------------------------------------------*/

#define CTNR_OK		( 0)
#define CTNR_ERR	(-1)

#define ctnr_success(ret) ((ret) >= 0)

/*-------------------------------------------------------------------------*/

typedef int ret_t;

/*-------------------------------------------------------------------------*/

typedef uint32_t enum_t;

/*-------------------------------------------------------------------------*/

typedef /* */ void *buff_t;
typedef const void *BUFF_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_thread_s
{
	uint8_t backend[64];

} ctnr_thread_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_mutex_s
{
	uint8_t backend[64];

} ctnr_mutex_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_mcond_s
{
	uint8_t backend[64];

} ctnr_mcond_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_pool_s
{
	size_t size;

	struct ctnr_pool_item_s *list;

} ctnr_pool_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_data_s
{
	size_t size;

	struct ctnr_data_item_s *list;

} ctnr_data_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_vect_s
{
	buff_t buff;
	size_t size;

	size_t data_size;
	size_t cache_size;

} ctnr_vect_t;

/*-------------------------------------------------------------------------*/

#define CTNR_HASH_MAX 512

/*-------------------------------------------------------------------------*/

typedef struct ctnr_hash_s
{
	struct
	{
		int name;
		int prev;
		int next;

	} offsets;

	void *dict[CTNR_HASH_MAX];

} ctnr_hash_t;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_buffer_s
{
	buff_t buff;
	size_t size;

	enum ctnr_buffer_method_e
	{
		CTNR_BUFFER_METHOD_NOALLOC
		,
		CTNR_BUFFER_METHOD_MALLOC
		,
		CTNR_BUFFER_METHOD_MMAP

	} method;

} ctnr_buffer_t;

/*-------------------------------------------------------------------------*/

CTNR_BEGIN_EXTERN_C

/*-------------------------------------------------------------------------*/
/* PANIC								   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC void ctnr_panic(const char *fmt, ...)
					__attribute__ ((format(printf, 1, 2), noreturn));

/*-------------------------------------------------------------------------*/
/* MEMORY								   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC size_t ctnr_memory_get_size(buff_t);
DLL_PUBLIC const char *ctnr_memory_get_buffer(buff_t);
DLL_PUBLIC int ctnr_memory_get_line(buff_t);

/*-------------------------------------------------------------------------*/

#define ctnr_new(x) \
		ctnr_cast(x *, __ctnr_memory_malloc((1) * sizeof(x), __FILE__, __LINE__))
#define ctnr_new_zero(x) \
		ctnr_cast(x *, __ctnr_memory_malloc_zero((1) * sizeof(x), __FILE__, __LINE__))

#define ctnr_new_array(x, n) \
		ctnr_cast(x *, __ctnr_memory_malloc((n) * sizeof(x), __FILE__, __LINE__))
#define ctnr_new_array_zero(x, n) \
		ctnr_cast(x *, __ctnr_memory_malloc_zero((n) * sizeof(x), __FILE__, __LINE__))

/*-------------------------------------------------------------------------*/

#define ctnr_memory_malloc(x) \
		__ctnr_memory_malloc(x, __FILE__, __LINE__)
#define ctnr_memory_malloc_zero(x) \
		__ctnr_memory_malloc_zero(x, __FILE__, __LINE__)

#define ctnr_memory_realloc(x, y) \
		__ctnr_memory_realloc(x, y, __FILE__, __LINE__)
#define ctnr_memory_realloc_zero(x, y) \
		__ctnr_memory_realloc_zero(x, y, __FILE__, __LINE__)

#define ctnr_memory_free(x) \
		__ctnr_memory_free(x, __FILE__, __LINE__)
#define ctnr_memory_free_zero(x) \
		__ctnr_memory_free_zero(x, __FILE__, __LINE__)

/*-------------------------------------------------------------------------*/

DLL_PUBLIC buff_t __ctnr_memory_malloc(size_t, const char *, int);
DLL_PUBLIC buff_t __ctnr_memory_malloc_zero(size_t, const char *, int);

DLL_PUBLIC buff_t __ctnr_memory_realloc(buff_t, size_t, const char *, int);
DLL_PUBLIC buff_t __ctnr_memory_realloc_zero(buff_t, size_t, const char *, int);

DLL_PUBLIC size_t __ctnr_memory_free(buff_t, const char *, int);
DLL_PUBLIC size_t __ctnr_memory_free_zero(buff_t, const char *, int);

/*-------------------------------------------------------------------------*/
/* POOL									   */
/*-------------------------------------------------------------------------*/

#define ctnr_pool_init(pool) \
		(pool)[0] = ((struct ctnr_pool_s) {0, NULL})

/*-------------------------------------------------------------------------*/

#define ctnr_pool_new(pool, x) \
		ctnr_cast(x *, __ctnr_pool_malloc(pool, (1) * sizeof(x), __FILE__, __LINE__))
#define ctnr_pool_new_zero(pool, x) \
		ctnr_cast(x *, __ctnr_pool_malloc_zero(pool, (1) * sizeof(x), __FILE__, __LINE__))

#define ctnr_pool_new_array(pool, x, n) \
		ctnr_cast(x *, __ctnr_pool_malloc(pool, (n) * sizeof(x), __FILE__, __LINE__))
#define ctnr_pool_new_array_zero(pool, x, n) \
		ctnr_cast(x *, __ctnr_pool_malloc_zero(pool, (n) * sizeof(x), __FILE__, __LINE__))

/*-------------------------------------------------------------------------*/

#define ctnr_pool_malloc(pool, x) \
		__ctnr_pool_malloc(pool, x, \
					__FILE__, __LINE__)
#define ctnr_pool_malloc_zero(pool, x) \
		__ctnr_pool_malloc_zero(pool, x, \
					__FILE__, __LINE__)

#define ctnr_pool_realloc(pool, x, y) \
		__ctnr_pool_realloc(pool, x, y, \
					__FILE__, __LINE__)
#define ctnr_pool_realloc_zero(pool, x, y) \
		__ctnr_pool_realloc_zero(pool, x, y, \
					__FILE__, __LINE__)

#define ctnr_pool_free(pool, x) \
		__ctnr_pool_free(pool, x, \
					__FILE__, __LINE__)
#define ctnr_pool_free_zero(pool, x) \
		__ctnr_pool_free_zero(pool, x, \
					__FILE__, __LINE__)

#define ctnr_pool_flush(pool) \
		__ctnr_pool_flush(pool, \
					__FILE__, __LINE__)
#define ctnr_pool_flush_zero(pool) \
		__ctnr_pool_flush_zero(pool, \
					__FILE__, __LINE__)

/*-------------------------------------------------------------------------*/

DLL_PUBLIC buff_t __ctnr_pool_malloc(struct ctnr_pool_s *, size_t, const char *, int);
DLL_PUBLIC buff_t __ctnr_pool_malloc_zero(struct ctnr_pool_s *, size_t, const char *, int);

DLL_PUBLIC buff_t __ctnr_pool_realloc(struct ctnr_pool_s *, buff_t, size_t, const char *, int);
DLL_PUBLIC buff_t __ctnr_pool_realloc_zero(struct ctnr_pool_s *, buff_t, size_t, const char *, int);

DLL_PUBLIC size_t __ctnr_pool_free(struct ctnr_pool_s *, buff_t, const char *, int);
DLL_PUBLIC size_t __ctnr_pool_free_zero(struct ctnr_pool_s *, buff_t, const char *, int);

DLL_PUBLIC size_t __ctnr_pool_flush(struct ctnr_pool_s *, const char *, int);
DLL_PUBLIC size_t __ctnr_pool_flush_zero(struct ctnr_pool_s *, const char *, int);

/*-------------------------------------------------------------------------*/
/* LIBRARY								   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC void *ctnr_library_open(const char *);

DLL_PUBLIC void *ctnr_library_load(void *, const char *);

DLL_PUBLIC void *ctnr_library_close(void *);

/*-------------------------------------------------------------------------*/
/* THREAD								   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC void ctnr_thread_create(ctnr_thread_t *,
					void *(*)(void *), void *);
DLL_PUBLIC void *ctnr_thread_waitfor(ctnr_thread_t *);

/**/

DLL_PUBLIC void ctnr_thread_exit(void *)
				__attribute__ ((noreturn));

/*-------------------------------------------------------------------------*/

DLL_PUBLIC void ctnr_mutex_allocate(struct ctnr_mutex_s *);
DLL_PUBLIC void ctnr_mutex_deallocate(struct ctnr_mutex_s *);

DLL_PUBLIC void ctnr_mutex_lock(struct ctnr_mutex_s *);
DLL_PUBLIC void ctnr_mutex_unlock(struct ctnr_mutex_s *);

/*-------------------------------------------------------------------------*/

DLL_PUBLIC void ctnr_mcond_allocate(struct ctnr_mcond_s *);
DLL_PUBLIC void ctnr_mcond_deallocate(struct ctnr_mcond_s *);

DLL_PUBLIC void ctnr_mcond_lock(struct ctnr_mcond_s *);
DLL_PUBLIC void ctnr_mcond_unlock(struct ctnr_mcond_s *);

DLL_PUBLIC void ctnr_mcond_wait(struct ctnr_mcond_s *);

DLL_PUBLIC void ctnr_mcond_signal(struct ctnr_mcond_s *);
DLL_PUBLIC void ctnr_mcond_broadcast(struct ctnr_mcond_s *);

/*-------------------------------------------------------------------------*/
/* DATA									   */
/*-------------------------------------------------------------------------*/

#define ctnr_data_init(data) \
		(data)[0] = ((struct ctnr_data_s) {0, NULL})

/*-------------------------------------------------------------------------*/

DLL_PUBLIC bool ctnr_data_push(struct ctnr_data_s *, BUFF_t, size_t, enum_t);

DLL_PUBLIC bool ctnr_data_pop_head(struct ctnr_data_s *, buff_t, size_t *, enum_t *);
DLL_PUBLIC bool ctnr_data_pop_tail(struct ctnr_data_s *, buff_t, size_t *, enum_t *);

DLL_PUBLIC bool ctnr_data_get_head(struct ctnr_data_s *, buff_t, size_t *, enum_t *);
DLL_PUBLIC bool ctnr_data_get_tail(struct ctnr_data_s *, buff_t, size_t *, enum_t *);

DLL_PUBLIC void ctnr_data_append(struct ctnr_data_s *, struct ctnr_data_s *);

DLL_PUBLIC size_t ctnr_data_clear(struct ctnr_data_s *);

/*-------------------------------------------------------------------------*/
/* VECTOR								   */
/*-------------------------------------------------------------------------*/

#define ctnr_vect_init(vect, type) \
		(vect)[0] = ((struct ctnr_vect_s) {NULL, 0x00, sizeof(type), /**/64/**/})

#define ctnr_vect_init_with_cache_size(vect, type, cache_size) \
		(vect)[0] = ((struct ctnr_vect_s) {NULL, 0x00, sizeof(type), cache_size})

/*-------------------------------------------------------------------------*/

DLL_PUBLIC void ctnr_vect_clear(struct ctnr_vect_s *);
DLL_PUBLIC void ctnr_pool_vect_clear(struct ctnr_pool_s *, struct ctnr_vect_s *);

DLL_PUBLIC bool ctnr_vect_push_back(struct ctnr_vect_s *, BUFF_t);
DLL_PUBLIC bool ctnr_pool_vect_push_back(struct ctnr_pool_s *, struct ctnr_vect_s *, BUFF_t);

DLL_PUBLIC bool ctnr_vect_pop_back(struct ctnr_vect_s *, buff_t);
DLL_PUBLIC bool ctnr_pool_vect_pop_back(struct ctnr_pool_s *, struct ctnr_vect_s *, buff_t);

DLL_PUBLIC void ctnr_vect_del(struct ctnr_vect_s *, size_t);
DLL_PUBLIC void ctnr_pool_vect_del(struct ctnr_pool_s *, struct ctnr_vect_s *, size_t);

/*-------------------------------------------------------------------------*/

#define ctnr_vect_size(vect) \
		(vect)->size

#define ctnr_pool_vect_size(pool, vect) \
		(vect)->size

#define ctnr_vect_at(vect, type, index) \
		ctnr_cast(type *, __ctnr_vect_at(vect, index))[0]

#define ctnr_pool_vect_at(pool, vect, type, index) \
		ctnr_cast(type *, __ctnr_pool_vect_at(pool, vect, index))[0]

/*-------------------------------------------------------------------------*/

DLL_PUBLIC buff_t __ctnr_vect_at(struct ctnr_vect_s *, size_t);
DLL_PUBLIC buff_t __ctnr_pool_vect_at(struct ctnr_pool_s *, struct ctnr_vect_s *, size_t);

/*-------------------------------------------------------------------------*/
/* HASH FUNCTIONS							   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC unsigned ctnr_crc32(BUFF_t, size_t, unsigned);
DLL_PUBLIC unsigned ctnr_murmur32(BUFF_t, size_t, unsigned);

/*-------------------------------------------------------------------------*/
/* HASH LIST								   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC unsigned ctnr_hash(const char *);

/*-------------------------------------------------------------------------*/

#define ctnr_hash_init(list, type, key) \
				__ctnr_hash_init(list, offsetof(type, key), offsetof(type, prev), offsetof(type, next))

#define ctnr_hash_init_named(list, type, key, prev, next) \
				__ctnr_hash_init(list, offsetof(type, key), offsetof(type, prev), offsetof(type, next))

/*-------------------------------------------------------------------------*/

DLL_PUBLIC void __ctnr_hash_init(struct ctnr_hash_s *, int, int, int);

/*-------------------------------------------------------------------------*/

DLL_PUBLIC void ctnr_hash_add(struct ctnr_hash_s *, void *);
DLL_PUBLIC void ctnr_hash_del(struct ctnr_hash_s *, void *);

DLL_PUBLIC bool ctnr_hash_lookup_item_by_hash(struct ctnr_hash_s *, void *, unsigned);
DLL_PUBLIC bool ctnr_hash_lookup_item_by_name(struct ctnr_hash_s *, void *, const char *);

DLL_PUBLIC void *ctnr_hash_lookup_entries_by_hash(struct ctnr_hash_s *, unsigned);
DLL_PUBLIC void *ctnr_hash_lookup_entries_by_name(struct ctnr_hash_s *, const char *);

/*-------------------------------------------------------------------------*/
/* BUFFER								   */
/*-------------------------------------------------------------------------*/

typedef enum ctnr_buffer_mode_e
{
	CTNR_BUFFER_MODE_READ,
	CTNR_BUFFER_MODE_WRITE,
	CTNR_BUFFER_MODE_BOTH,

} ctnr_buffer_mode_t;

/*-------------------------------------------------------------------------*/

DLL_PUBLIC bool ctnr_buffer_free(struct ctnr_buffer_s *);

/*-------------------------------------------------------------------------*/

DLL_PUBLIC bool ctnr_buffer_from_buff(struct ctnr_buffer_s *, buff_t, size_t, bool);

DLL_PUBLIC bool ctnr_buffer_from_file(struct ctnr_buffer_s *, const char *, enum ctnr_buffer_mode_e, bool);

/*-------------------------------------------------------------------------*/

DLL_PUBLIC bool ctnr_buffer_save_to_file(struct ctnr_buffer_s *, const char *);

/*-------------------------------------------------------------------------*/
/* DEFLATE								   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC bool ctnr_deflate(buff_t, size_t *, BUFF_t, size_t);
DLL_PUBLIC bool ctnr_inflate(buff_t, size_t *, BUFF_t, size_t);

/*-------------------------------------------------------------------------*/
/* BSD									   */
/*-------------------------------------------------------------------------*/

DLL_PUBLIC int ctnr_snprintf(char *, size_t, const char *, ...);

/*-------------------------------------------------------------------------*/

DLL_PUBLIC char *ctnr_strdup(const char *);
DLL_PUBLIC char *ctnr_pool_strdup(struct ctnr_pool_s *, const char *);

DLL_PUBLIC char *ctnr_strndup(const char *, size_t);
DLL_PUBLIC char *ctnr_pool_strndup(struct ctnr_pool_s *, const char *, size_t);

/*-------------------------------------------------------------------------*/

CTNR_END_EXTERN_C

/*-------------------------------------------------------------------------*/

#ifdef __OBJC__

  /*-----------------------------------------------------------------------*/

  #ifndef __NEXT_RUNTIME__
    #if __GNUC__ == 4 && __GNUC_MINOR__ < 6
      #error "LIBCTNR for Objective-C requires at least gcc 4.6.0"
    #endif
  #endif

  /*-----------------------------------------------------------------------*/

  #include <objc/objc.h>

  /*-----------------------------------------------------------------------*/

  typedef Class cl;

  typedef const char *ID;
  typedef const char *CL;

  /*-----------------------------------------------------------------------*/

  @class TMethodSignature;

  /*-----------------------------------------------------------------------*/

  #define CTNR_OBJC_INIT \
		if((self = [super init]) == nil)	\
		{					\
			return nil;			\
		}

  /*-----------------------------------------------------------------------*/

  #define CTNR_OBJC_IS_SINGLETON(object) [(object) isSubclassOfClass: [TSingleton class]]

  /*-----------------------------------------------------------------------*/

  DLL_PUBLIC cl ctnr_objc_class(const char *);
  DLL_PUBLIC id ctnr_objc_object(const char *);
  DLL_PUBLIC id ctnr_objc_singleton(const char *);

  /*-----------------------------------------------------------------------*/
  /* TObject								   */
  /*-----------------------------------------------------------------------*/

  @interface TObject
  {
    @public
	Class isa;

	TObject *m_parent;
	TObject *m_prev;
	TObject *m_next;

    @protected
	TObject *m_list;

	ctnr_pool_t m_pool;

    @private
	volatile int m_refcnt;
  }

  /**/

  + (void) initialize;

  /**/

  - (id) init;

  + (id) alloc;

  - (id) dealloc;

  /**/

  - (/*  */ id) retain;
  - (oneway id) release;

  - (int) retainCount;

  /**/

  - (buff_t) memAlloc: (size_t) size;
  - (buff_t) memAllocZero: (size_t) size;

  - (buff_t) memRealloc: (buff_t) buff: (size_t) size;
  - (buff_t) memReallocZero: (buff_t) buff: (size_t) size;

  - (size_t) memFree: (buff_t) buff;
  - (size_t) memFreeZero: (buff_t) buff;

  - (char *) strDup: (const char *) str;
  - (char *) strNDup: (const char *) str: (size_t) size;

  /**/

  - (id) poolAdd: (TObject *) anObject;
  - (id) poolDel: (TObject *) anObject;

  - (void) poolFlush;

  /**/

  - (id) self;

  + (Class) class;
  - (Class) class;

  + (Class) superclass;
  - (Class) superclass;

  - (const char *) className;
  - (const char *) uniqueName;

  /**/

  + (bool) isSubclassOfClass: (Class) aClass;

  /**/

  + (bool) instancesRespondToSelector: (SEL) aSelector;
  - (bool) respondToSelector: (SEL) aSelector;

  + (IMP) instanceMethodForSelector: (SEL) aSelector;
  - (IMP) methodForSelector: (SEL) aSelector;

  + (TMethodSignature *) instanceMethodSignatureForSelector: (SEL) aSelector;
  - (TMethodSignature *) methodSignatureForSelector: (SEL) aSelector;

  @end

  /*-----------------------------------------------------------------------*/
  /* TSingleton								   */
  /*-----------------------------------------------------------------------*/

  @interface TSingleton: TObject

  @end

  /*-----------------------------------------------------------------------*/
  /* TMethodSignature							   */
  /*-----------------------------------------------------------------------*/

  @interface TMethodSignature: TObject
  {
    @private
	int m_nr;

	char *m_type_array[512];
  }

  /**/

  + (TMethodSignature *) signatureWithObjCTypes: (const char *) types;

  /**/

  - (unsigned int) getNr;

  - (const char *) getReturnType;

  - (const char *) getArgumentType: (unsigned int) index;

  @end

  /*-----------------------------------------------------------------------*/

#endif

/*-------------------------------------------------------------------------*/

#endif /* __CTNR_H */

/*-------------------------------------------------------------------------*/

