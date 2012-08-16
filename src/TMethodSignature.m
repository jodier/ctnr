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

#include <ctype.h>

#include <objc/runtime.h>

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/
/* PARSER								   */
/*-------------------------------------------------------------------------*/

#define _C_CONST	'r'
#define _C_IN		'n'
#define _C_INOUT	'N'
#define _C_OUT		'o'
#define _C_BYCOPY	'O'
#define _C_BYREF	'R'
#define _C_ONEWAY	'V'
#define _C_GCINVISIBLE	'|'

/*-------------------------------------------------------------------------*/

static const char *__skip_qualifiers(register const char *type)
{
	while(*type == _C_CONST
	      ||
	      *type == _C_IN
	      ||
	      *type == _C_INOUT
	      ||
	      *type == _C_OUT
	      ||
	      *type == _C_BYCOPY
	      ||
	      *type == _C_BYREF
	      ||
	      *type == _C_ONEWAY
	      ||
	      *type == _C_GCINVISIBLE
	 ) {
		type++;
	}

	return type;
}

/*-------------------------------------------------------------------------*/

static const char *__skip_offset(register const char *type)
{
	if(*type == '+') {
		type++;
	}

	if(*type == '-') {
		type++;
	}

	while(isdigit(*type) != false)
	{
		type++;
	}

	return type;
}

/*-------------------------------------------------------------------------*/

static const char *__skip_type(register const char *type)
{
	type = __skip_qualifiers(type);

	switch(*type)
	{
		case _C_PTR:
			type++;
			break;

		case _C_ARY_B:
			while(*type != _C_ARY_E)
			{
				type++;
			}
			break;

		case _C_STRUCT_B:
			while(*type != _C_STRUCT_E)
			{
				type++;
			}
			break;

		case _C_UNION_B:
			while(*type != _C_UNION_E)
			{
				type++;
			}
			break;
	}

	return ++type;
}

/*-------------------------------------------------------------------------*/

static const char *__process(const char *type)
{
	type = __skip_type(type);

	type = __skip_offset(type);

	return type;
}

/*-------------------------------------------------------------------------*/
/* TMethodSignature							   */
/*-------------------------------------------------------------------------*/

@implementation TMethodSignature

/*-------------------------------------------------------------------------*/

+ (TMethodSignature *) signatureWithObjCTypes: (const char *) types;
{
	TMethodSignature *result = [[TMethodSignature alloc] init];

	/**/

	for(; types[0] != '\0'; types = __process(types))
	{
		size_t size = ctnr_cast(size_t, __skip_type(types) - types);

		result->m_type_array[result->m_nr++] = [result strNDup: types: size];
	}

	return result;
}

/*-------------------------------------------------------------------------*/

- (unsigned int) getNr
{
	return self->m_nr - 1;
}

/*-------------------------------------------------------------------------*/

- (const char *) getReturnType
{
	return self->m_type_array[0];
}

/*-------------------------------------------------------------------------*/

- (const char *) getArgumentType: (unsigned int) index
{
	index++;

	return self->m_nr > index ? self->m_type_array[index] : "";
}

/*-------------------------------------------------------------------------*/

@end

/*-------------------------------------------------------------------------*/

