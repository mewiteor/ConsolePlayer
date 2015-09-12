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
#include "ErrorDiffusion.h"
#include<cstring>

CErrorDiffusion::CErrorDiffusion(unsigned int width, unsigned int height)
	:CDither(width, height)
	, m_clrErrors(nullptr)
{
	if (width&&height)
		m_clrErrors = new COLOR[width*height];
}

CErrorDiffusion::~CErrorDiffusion()
{
	if (m_clrErrors)
	{
		delete[] m_clrErrors;
		m_clrErrors = nullptr;
	}
}

void CErrorDiffusion::Init()
{
	if (m_clrErrors)
		memset(m_clrErrors, 0, sizeof(COLOR)*m_nWidth*m_nHeight);
}

COLORREF CErrorDiffusion::Get(unsigned char red, unsigned char green, unsigned char blue, size_t i)
{
	auto c = RGB(red, green, blue);
	if (!m_clrErrors)return c;
	auto t = m_clrErrors[i] + COLOR(c) + 0.5;
	auto gred = unsigned char(t.red < 0 ? 0 : t.red>255 ? 255 : (int)t.red);
	auto ggreen = unsigned char(t.green < 0 ? 0 : t.green>255 ? 255 : (int)t.green);
	auto gblue = unsigned char(t.blue < 0 ? 0 : t.blue>255 ? 255 : (int)t.blue);
	return RGB(gred, ggreen, gblue);
}

CErrorDiffusion::COLOR::COLOR(const COLORREF & color)
	:red(GetRValue(color))
	,green(GetGValue(color))
	,blue(GetBValue(color))
{
}

CErrorDiffusion::COLOR & CErrorDiffusion::COLOR::operator+=(const COLOR & color)
{
	red += color.red;
	green += color.green;
	blue += color.blue;
	return *this;
}

CErrorDiffusion::COLOR & CErrorDiffusion::COLOR::operator-=(const COLOR & color)
{
	red -= color.red;
	green -= color.green;
	blue -= color.blue;
	return *this;
}

CErrorDiffusion::COLOR CErrorDiffusion::COLOR::operator+(const COLOR & color) const
{
	return COLOR(*this) += color;
}

CErrorDiffusion::COLOR CErrorDiffusion::COLOR::operator-(const COLOR & color) const
{
	return COLOR(*this) -= color;
}

CErrorDiffusion::COLOR & CErrorDiffusion::COLOR::operator+=(const _REAL & num)
{
	red += num;
	green += num;
	blue += num;
	return *this;
}

CErrorDiffusion::COLOR CErrorDiffusion::COLOR::operator+(const _REAL & num)const
{
	return COLOR(*this) += num;
}

CErrorDiffusion::COLOR & CErrorDiffusion::COLOR::operator*=(const _REAL & num)
{
	red *= num;
	green *= num;
	blue *= num;
	return *this;
}

CErrorDiffusion::COLOR & CErrorDiffusion::COLOR::operator/=(const _REAL & num)
{
	red /= num;
	green /= num;
	blue /= num;
	return *this;
}

CErrorDiffusion::COLOR CErrorDiffusion::COLOR::operator*(const _REAL & num) const
{
	return COLOR(*this) *= num;
}

CErrorDiffusion::COLOR CErrorDiffusion::COLOR::operator/(const _REAL & num) const
{
	return COLOR(*this) /= num;
}
