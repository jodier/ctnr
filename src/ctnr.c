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

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

void __ctnr_memory_ctor(void);
void __ctnr_memory_dtor(void);
#ifdef HAVE_LIBOBJC
void __ctnr_objc_ctor(void);
void __ctnr_objc_dtor(void);
#endif
void __ctnr_library_ctor(void);
void __ctnr_library_dtor(void);

/*-------------------------------------------------------------------------*/

static void __CTNR_CTOR __ctnr_ctor(void)
{
	__ctnr_memory_ctor();
#ifdef HAVE_LIBOBJC
	__ctnr_objc_ctor();
#endif
	__ctnr_library_ctor();
}

/*-------------------------------------------------------------------------*/

static void __CTNR_DTOR __ctnr_dtor(void)
{
	fflush(stdout);
	fflush(stderr);

	__ctnr_memory_dtor();
#ifdef HAVE_LIBOBJC
	__ctnr_objc_dtor();
#endif
	__ctnr_library_dtor();
}

/*-------------------------------------------------------------------------*/

