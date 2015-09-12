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
#include "ColorQuantization.h"

#include<cstring>

CColorQuantization::Node CColorQuantization::m_cNodes[0111111111];

CColorQuantization::CColorQuantization(unsigned int max)
	:m_nColorCount(0)
	, m_nMaxCount(max)
	, m_nDepth(8)
{
	memset(m_cNodes, 0, sizeof(Node));
	m_cNodes[0].leaf = true;
}

void CColorQuantization::Insert(unsigned char red, unsigned char green, unsigned char blue)
{
	InsertRGB(red, green, blue);
	while (m_nColorCount > m_nMaxCount)
		Reduce();
}

unsigned int CColorQuantization::Done(COLORREF colorTable[16])
{
	unsigned int index = 0;
	for (const auto &it : m_setLeafParent)
		for (unsigned int i = (it << 3) + 1, j = i + 8; i < j; ++i)
			if (m_cNodes[i].count&&m_cNodes[i].leaf)
			{
				m_cNodes[i].red = (m_cNodes[i].red + (m_cNodes[i].count >> 1)) / m_cNodes[i].count;
				m_cNodes[i].green = (m_cNodes[i].green + (m_cNodes[i].count >> 1)) / m_cNodes[i].count;
				m_cNodes[i].blue = (m_cNodes[i].blue + (m_cNodes[i].count >> 1)) / m_cNodes[i].count;
				m_cNodes[i].count = 1;
				m_cNodes[i].index = index;
				colorTable[index++] = RGB(m_cNodes[i].red, m_cNodes[i].green, m_cNodes[i].blue);
			}
	return index;
}

unsigned int CColorQuantization::Get(unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned int index;
	for (index = 0; !m_cNodes[index].leaf; red <<= 1, green <<= 1, blue <<= 1)
		index = (index << 3) + 1 + (red >> 7 << 2 | green >> 7 << 1 | blue >> 7);
	return m_cNodes[index].index;
}

void CColorQuantization::InsertRGB(unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned int i, index;
	for (i = 0, index = 0; i < m_nDepth; ++i)
	{
		if (m_cNodes[index].leaf)
		{
			if (m_cNodes[index].count)
			{
				m_cNodes[index].red += red;
				m_cNodes[index].green += green;
				m_cNodes[index].blue += blue;
				++m_cNodes[index].count;
				break;
			}
			m_cNodes[index].leaf = false;
			memset(m_cNodes + (index << 3) + 1, 0, sizeof(Node) * 8);
			for (size_t i = (index << 3) + 1, j = 0; j < 8; ++i, ++j)
				m_cNodes[i].leaf = true;
		}
		++m_cNodes[index].count;
		index = (index << 3) + 1 +
			((red >> (7 - i) & 1) << 2 |
				(green >> (7 - i) & 1) << 1 |
				(blue >> (7 - i) & 1));
	}
	if (i == m_nDepth)
	{
		if (!m_cNodes[index].count)
		{
			++m_nColorCount;
			m_setLeafParent.insert((index - 1) >> 3);
		}
		m_cNodes[index].red += red;
		m_cNodes[index].green += green;
		m_cNodes[index].blue += blue;
		++m_cNodes[index].count;
	}
}

void CColorQuantization::Reduce()
{
	auto it = m_setLeafParent.rbegin();
	unsigned int max_index = *it;

	// 类似于 0111...111这样的八进制数
	unsigned int min_index =
		((1 << (fast_log2(max_index - (max_index >> 3)) / 3 * 3 + 3)) - 1) / 7;
	unsigned long long min_count = m_cNodes[max_index].count;
	decltype(it) mit = it;
	while (++it != m_setLeafParent.rend())	// 找count最小的m_setLeafParent结点
	{
		auto t = *it;
		if (t < min_index)
			break;
		if (min_count > m_cNodes[t].count)
		{
			min_count = m_cNodes[t].count;
			mit = it;
		}
	}
	auto index = *mit;	// count最小的m_setLeafParent结点的索引
	m_cNodes[index].leaf = true;
	for (unsigned int j = (index << 3) + 1, i = j + 8; j < i; ++j)
		if (m_cNodes[j].count)
		{
			m_cNodes[index].red += m_cNodes[j].red;
			m_cNodes[index].green += m_cNodes[j].green;
			m_cNodes[index].blue += m_cNodes[j].blue;
			--m_nColorCount;
		}
	m_setLeafParent.insert((index - 1) >> 3);
	++m_nColorCount;
	m_setLeafParent.erase((++mit).base());
	auto last = *m_setLeafParent.rbegin();
	m_nDepth = fast_log2(last - (last >> 3)) / 3 + 2;
}
