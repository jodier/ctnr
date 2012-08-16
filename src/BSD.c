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
#define _BSD_SOURCE
/*-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/
/* STDIO.H								   */
/*-------------------------------------------------------------------------*/

int ctnr_snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int result = vsnprintf(str, size, format, ap);
	va_end(ap);

	return result;
}

/*-------------------------------------------------------------------------*/
/* STRING.H								   */
/*-------------------------------------------------------------------------*/

char *ctnr_strdup(const char *src)
{
	size_t l = strlen(src) + 1;

	return ctnr_cast(char *, memcpy(ctnr_new_array(char, l), src, l));
}

/*-------------------------------------------------------------------------*/

char *ctnr_pool_strdup(ctnr_pool_t *pool, const char *src)
{
	size_t l = strlen(src) + 1;

	return ctnr_cast(char *, memcpy(ctnr_pool_new_array(pool, char, l), src, l));
}

/*-------------------------------------------------------------------------*/

char *ctnr_strndup(const char *src, size_t n)
{
	size_t l = strlen(src);

	if(l > n)
	{
		l = n;
	}

	char *result = ctnr_new_array(char, l + 1);

	result[l] = '\0';

	return ctnr_cast(char *, memcpy(result, src, l));
}

/*-------------------------------------------------------------------------*/

char *ctnr_pool_strndup(ctnr_pool_t *pool, const char *src, size_t n)
{
	size_t l = strlen(src);

	if(l > n)
	{
		l = n;
	}

	char *result = ctnr_pool_new_array(pool, char, l + 1);

	result[l] = '\0';

	return ctnr_cast(char *, memcpy(result, src, l));
}

/*-------------------------------------------------------------------------*/

