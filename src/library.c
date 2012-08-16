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

#ifdef __IS_WIN
  #include <windows.h>
#else
  #include  <dlfcn.h>
#endif

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

static ctnr_mutex_t mutex;

/*-------------------------------------------------------------------------*/

typedef struct ctnr_handle_leak_s
{
	void *handle;

	struct ctnr_handle_leak_s *prev, *next;

} ctnr_handle_leak_t;

/*-------------------------------------------------------------------------*/

static ctnr_handle_leak_t array[8192];

/*-------------------------------------------------------------------------*/

static ctnr_handle_leak_t *free_list = NULL;
static ctnr_handle_leak_t *used_list = NULL;

/*-------------------------------------------------------------------------*/

void *ctnr_library_open(const char *name)
{
#ifdef __IS_WIN
	void *result = LoadLibrary(name);
#else
	void *result = dlopen(name, RTLD_NOW);
#endif
	ctnr_mutex_lock(&mutex);

	/**/	if(free_list != NULL)
	/**/	{
	/**/		ctnr_handle_leak_t *leak = ctnr_list_get_head(free_list);
	/**/
	/**/		ctnr_list_remove(free_list, leak);
	/**/		ctnr_list_append(used_list, leak);
	/**/
	/**/		leak->handle = result;
	/**/	}

	ctnr_mutex_unlock(&mutex);

	return result;
}

/*-------------------------------------------------------------------------*/

void *ctnr_library_load(void *handle, const char *name)
{
	void *result;

	char buffer[1024];

	/**/

	if(handle != NULL)
	{
#ifdef __IS_WIN
		result = GetProcAddress(handle, name);
#else
		result = dlsym(handle, name);
#endif
		if(result == NULL)
		{
			ctnr_snprintf(buffer, 1024, "_%s", name);
#ifdef __IS_WIN
			result = GetProcAddress(handle, buffer);
#else
			result = dlsym(handle, buffer);
#endif
			if(result == NULL)
			{
				ctnr_snprintf(buffer, 1024, "__%s", name);
#ifdef __IS_WIN
				result = GetProcAddress(handle, buffer);
#else
				result = dlsym(handle, buffer);
#endif
				if(result == NULL)
				{
					ctnr_snprintf(buffer, 1024, "___%s", name);
#ifdef __IS_WIN
					result = GetProcAddress(handle, buffer);
#else
					result = dlsym(handle, buffer);
#endif
				}
			}
		}
	}
	else {
		result = NULL;
	}

	return result;
}

/*-------------------------------------------------------------------------*/

void *ctnr_library_close(void *handle)
{
	if(handle != NULL)
	{
		int nr;
		ctnr_handle_leak_t *leak;

		ctnr_mutex_lock(&mutex);

		/**/	ctnr_list_foreach(used_list, leak, nr)
		/**/	{
		/**/		if(leak->handle == handle)
		/**/		{
		/**/			ctnr_list_remove(used_list, leak);
		/**/			ctnr_list_append(free_list, leak);
		/**/
		/**/			leak->handle = NULL;
		/**/
		/**/			break;
		/**/		}
		/**/	}

		ctnr_mutex_unlock(&mutex);
#ifdef __IS_WIN
		FreeLibrary(handle);
#else
		dlclose(handle);
#endif
	}

	return NULL;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

DLL_HIDDEN void __ctnr_library_ctor(void)
{
	ctnr_mutex_allocate(&mutex);

	/**/

	int i;
	ctnr_handle_leak_t *leak;

	for(i = 0; i < 8192; i++)
	{
		leak = &array[i];

		ctnr_list_append(free_list, leak);
	}
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

DLL_HIDDEN void __ctnr_library_dtor(void)
{
	int nr;
	ctnr_handle_leak_t *leak;

	ctnr_list_foreach(used_list, leak, nr)
	{
		if(leak->handle != NULL)
		{
#ifdef __IS_WIN
			FreeLibrary(leak->handle);
#else
			dlclose(leak->handle);
#endif
		}
	}

	/**/

	ctnr_mutex_deallocate(&mutex);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

