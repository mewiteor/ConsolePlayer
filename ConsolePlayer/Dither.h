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
 /*!
  * \class CDither
  *
  * \brief 图像抖动类
  *
  * \author Mewiteor
  * \date 九月 2015
  */
class CDither
{
public:

	//************************************
	// Method:      CDither
	// FullName:    CDither::CDither
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   unsigned int width
	// Parameter:   unsigned int height
	// Description: 构造抖动对象
	//************************************
	CDither(unsigned int width, unsigned int height);

	//************************************
	// Method:      ~CDither
	// FullName:    CDither::~CDither
	// Access:      virtual public 
	// Returns:     
	// Qualifier:  
	// Description: 析构抖动对象
	//************************************
	virtual ~CDither();
protected:
	const unsigned int m_nWidth;	//图像宽度
	const unsigned int m_nHeight;	//图像高度
};

