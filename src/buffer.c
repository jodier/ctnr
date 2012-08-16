/* Author  : Jerome ODIER
 * Email   : jerome.odier@cern.ch
 *
 * Version : 1.0 (2007-2012)
 *
 *
 * This buffer is part of LIBCTNR.
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

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#ifdef __IS_WIN
  #include <windows.h>
#else
  #include <sys/mman.h>
#endif

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

#ifdef __IS_WIN

/*-------------------------------------------------------------------------*/

#define PROT_READ	(1 << 0)
#define PROT_WRITE	(1 << 1)
#define PROT_NONE	(1 << 2)

#define MAP_SHARED	(1 << 0)
#define MAP_PRIVATE	(1 << 1)

#define MAP_FAILED	((NULL))

/*-------------------------------------------------------------------------*/

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	DWORD __prot = 0x00;
	DWORD __flags = 0x00;

	if((prot & PROT_READ) != 0)
	{
		__prot = PAGE_READONLY;
		__flags = FILE_MAP_READ;
	}

	if((prot & PROT_WRITE) != 0)
	{
		/**/ if(flags == MAP_PRIVATE)
		{
			__prot = PAGE_WRITECOPY;
			__flags = FILE_MAP_COPY;
		} 
		else if(flags == MAP_SHARED)
		{
			__prot = PAGE_READWRITE;
			__flags = FILE_MAP_WRITE;
		}
	}

	/**/

	HANDLE hmap = CreateFileMapping((HANDLE) _get_osfhandle(fd), NULL, __prot, 0, 0, NULL);

	if(hmap == NULL)
	{
		return MAP_FAILED;
	}

	/**/

	void *result = MapViewOfFileEx(hmap, __flags, 0, 0, length, addr);

	return CloseHandle(hmap) != FALSE && result != NULL ? ((result))
							    : MAP_FAILED
							    ;
}

/*-------------------------------------------------------------------------*/

int munmap(void *addr, size_t size)
{
	return UnmapViewOfFile(addr) != 0 ? 0 : -1;
}

/*-------------------------------------------------------------------------*/

#endif

/*-------------------------------------------------------------------------*/

bool ctnr_buffer_free(ctnr_buffer_t *buffer)
{
	bool result = true;

	if(buffer->buff != NULL
	   &&
	   buffer->size != 0x00
	 ) {
		switch(buffer->method)
		{
			case CTNR_BUFFER_METHOD_MALLOC:
				ctnr_memory_free(buffer->buff);
				break;

			case CTNR_BUFFER_METHOD_MMAP:
				if(munmap(buffer->buff, buffer->size) < 0) result = false;
				break;

			default:
				break;
		}
	}

	buffer->buff = NULL;
	buffer->size = 0x00;

	return result;
}

/*-------------------------------------------------------------------------*/

bool ctnr_buffer_from_buff(ctnr_buffer_t *buffer, buff_t buff, size_t size, bool copy)
{
	if(copy != false)
	{
		buffer->buff = ctnr_memory_malloc(size);
		buffer->size = /*--------------*/(size);

		memcpy(buffer->buff, buff, size);

		buffer->method = CTNR_BUFFER_METHOD_MALLOC;
	}
	else
	{
		buffer->buff = buff;
		buffer->size = size;

/*		memcpy(buffer->buff, buff, size);
 */
		buffer->method = CTNR_BUFFER_METHOD_NOALLOC;
	}

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_buffer_from_file(ctnr_buffer_t *buffer, const char *fname, ctnr_buffer_mode_t mode, bool shared)
{
	/*-----------------------------------------------------------------*/

	int __mode;
	int __prot;
	
	switch(mode)
	{
		case CTNR_BUFFER_MODE_READ:
			__mode = O_RDONLY;
			__prot = PROT_READ;
			break;

		case CTNR_BUFFER_MODE_WRITE:
			__mode = O_WRONLY;
			__prot = PROT_WRITE;
			break;

		case CTNR_BUFFER_MODE_BOTH:
			__mode = O_RDWR;
			__prot = PROT_READ | PROT_WRITE;
			break;

		default:
			return false;
	}

	/*-----------------------------------------------------------------*/

	int __flag = shared ? MAP_SHARED : MAP_PRIVATE;

	/*-----------------------------------------------------------------*/

	int fd;

	struct stat buf;

	if(stat(fname, &buf) < 0
	   ||
	   S_ISREG(buf.st_mode) == 0
	   ||
	   (fd = open(fname, __mode)) < 0
	 ) {
		return false;
	}

	size_t size = buf.st_size;

	/*-----------------------------------------------------------------*/

	buff_t buff = mmap(NULL, size, __prot, __flag, fd, 0);

	if(buff == MAP_FAILED)
	{
		close(fd);

		return false;
	}

	/*-----------------------------------------------------------------*/

	buffer->buff = buff;
	buffer->size = size;

	buffer->method = CTNR_BUFFER_METHOD_MMAP;

	/*-----------------------------------------------------------------*/

	close(fd);

	return true;
}

/*-------------------------------------------------------------------------*/

bool ctnr_buffer_save_to_file(ctnr_buffer_t *buffer, const char *fname)
{
	FILE *fp = fopen(fname, "wb");

	if(fp == NULL)
	{
		return false;
	}

	/**/

	if(buffer->buff != NULL
	   &&
	   buffer->size != 0x00
	 ) {
		if(fwrite(buffer->buff, 1, buffer->size, fp) != buffer->size)
		{
			fclose(fp);

			return false;
		}
	}

	fclose(fp);

	return true;
}

/*-------------------------------------------------------------------------*/

