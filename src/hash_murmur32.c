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
 *
 */

/*-------------------------------------------------------------------------*/

#include "../include/ctnr.h"

/*-------------------------------------------------------------------------*/

#define MIX(h, k)	\
{			\
	k *= s;		\
	k ^= k >> t;	\
	k *= s;		\
			\
	h *= s;		\
	h ^= k;		\
}

/*-------------------------------------------------------------------------*/

unsigned ctnr_murmur32(BUFF_t buff, size_t size, unsigned seed)
{
	const unsigned int s = 0x5BD1E995;
	const  signed  int t = 0x00000018;

	/**/

	register const uint8_t *data = ctnr_cast(const uint8_t *, buff);

	unsigned h = seed ^ size;

	while(size >= 4)
	{
		uint32_t k = *ctnr_cast(uint32_t *, data);

		MIX(h, k);

		data += 4;
		size -= 4;
	}
	
	switch(size)
	{
		case 3:
			h ^= data[2] << 0x10;
		case 2:
			h ^= data[1] << 0x08;
		case 1:
			h ^= data[0] << 0x00;

			h *= s;
	}

	h ^= h >> 13;
	h *= s;
	h ^= h >> 15;

	return h;
}

/*-------------------------------------------------------------------------*/

