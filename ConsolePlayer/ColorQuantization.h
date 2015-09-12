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
#pragma once
#include<Windows.h>
#include<set>

/*!
 * \class CColorQuantization
 *
 * \brief 用八叉树算法进行色彩量化(24bpp->4bpp)
 *
 * \author Mewiteor
 * \date 九月 2015
 */
class CColorQuantization
{
	typedef std::set<unsigned int> set_type;	// set类型
	struct Node	// 结点结构
	{
		unsigned long long red;		// 红色总数
		unsigned long long green;	// 绿色总数
		unsigned long long blue;	// 蓝色总数
		unsigned long long count;	// 经过结点计数
		unsigned int index;			// 调色板索引
		bool leaf;					// 是否为叶子结点
	};
public:

	//************************************
	// Method:      CColorQuantization
	// FullName:    CColorQuantization::CColorQuantization
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   unsigned int max，最多颜色数
	// Description: 构造色彩量化对象
	//************************************
	CColorQuantization(unsigned int max);

	//************************************
	// Method:      ~CColorQuantization
	// FullName:    CColorQuantization::~CColorQuantization
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Description: 析构色彩量化对象
	//************************************
	~CColorQuantization() {}

	//************************************
	// Method:      Insert
	// FullName:    CColorQuantization::Insert
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   unsigned char red
	// Parameter:   unsigned char green
	// Parameter:   unsigned char blue
	// Description: 插入颜色到八叉堆中，并优化堆
	//************************************
	void Insert(unsigned char red, unsigned char green, unsigned char blue);

	//************************************
	// Method:      Done
	// FullName:    CColorQuantization::Done
	// Access:      public 
	// Returns:     unsigned int
	// Qualifier:  
	// Parameter:   COLORREF colorTable[16]
	// Description: 生成调色板
	//************************************
	unsigned int Done(COLORREF colorTable[16]);

	//************************************
	// Method:      Get
	// FullName:    CColorQuantization::Get
	// Access:      public 
	// Returns:     unsigned int
	// Qualifier:  
	// Parameter:   unsigned char red
	// Parameter:   unsigned char green
	// Parameter:   unsigned char blue
	// Description: 获取颜色在调色板中的最优索引
	//************************************
	unsigned int Get(unsigned char red, unsigned char green, unsigned char blue);

private:

	//************************************
	// Method:      InsertRGB
	// FullName:    CColorQuantization::InsertRGB
	// Access:      private 
	// Returns:     void
	// Qualifier:  
	// Parameter:   unsigned char red
	// Parameter:   unsigned char green
	// Parameter:   unsigned char blue
	// Description: 插入颜色到八叉堆中
	//************************************
	void InsertRGB(unsigned char red, unsigned char green, unsigned char blue);
	
	//************************************
	// Method:      Reduce
	// FullName:    CColorQuantization::Reduce
	// Access:      private 
	// Returns:     void
	// Qualifier:  
	// Description: 减少颜色
	//************************************
	void Reduce();
	
	//************************************
	// Method:      fast_log2
	// FullName:    CColorQuantization::fast_log2
	// Access:      private 
	// Returns:     unsigned short
	// Qualifier:  
	// Parameter:   double d
	// Description: 快速计算以2为底的对数
	//************************************
	inline unsigned short fast_log2(double d)
	{
		return (*(unsigned long long*)&d >> 52 & 0x7ff) - 1023;
	}

private:
	static Node m_cNodes[0111111111];	// 九层八叉堆
	unsigned int m_nColorCount;		// 颜色总数
	const unsigned int m_nMaxCount;	// 最多颜色数
	set_type m_setLeafParent;			// 叶子结点的父结点的集合
	unsigned int m_nDepth;				// 堆的当前深度
};

