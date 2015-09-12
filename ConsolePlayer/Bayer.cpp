/*
 * copyright (c) 2015 Mewiteor
 *
 * This file is part of ConsolePlayer.
 *
 * ConsolePlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ConsolePlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ConsolePlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "Bayer.h"

const unsigned char CBayer::m_nM[16] =
{
    0,8,2,10,
    12,4,14,6,
    3,11,1,9,
    15,7,13,5
};

CBayer::CBayer(unsigned int width, unsigned int height)
    :CDither(width, height)
{
}


CBayer::~CBayer()
{
}

unsigned char CBayer::Get(unsigned char alpha, size_t i)
{
    auto x = i%m_nWidth, y = i / m_nWidth;
    auto t = alpha >> 4, r = alpha & 15;
    if (r > m_nM[(y & 3) << 2 | x & 3] + t)++t;
    return t << 4 | t;
}
