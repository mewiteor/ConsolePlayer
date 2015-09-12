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
#include"ErrorDiffusion.h"
/*!
 * \class CMyErrorDiffusion
 *
 * \brief 自己定义的误差扩散算法，适用于3x5的像素
 *
 * \author Mewiteor
 * \date 九月 2015
 */
class CMyErrorDiffusion
	:public CErrorDiffusion
{
public:

	//************************************
	// Method:      CMyErrorDiffusion
	// FullName:    CMyErrorDiffusion::CMyErrorDiffusion
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   unsigned int width
	// Parameter:   unsigned int height
	// Description: 
	//************************************
	CMyErrorDiffusion(unsigned int width, unsigned int height);

	//************************************
	// Method:      ~CMyErrorDiffusion
	// FullName:    CMyErrorDiffusion::~CMyErrorDiffusion
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Description: 
	//************************************
	~CMyErrorDiffusion();

	//************************************
	// Method:      Diffusion
	// FullName:    CMyErrorDiffusion::Diffusion
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   unsigned char red
	// Parameter:   unsigned char green
	// Parameter:   unsigned char blue
	// Parameter:   size_t i
	// Parameter:   COLORREF color
	// Description: 误差扩散
	//				| ___ * 4 2 |
	//				| 1 2 6 2 1 |*1/18
	//************************************
	void Diffusion(unsigned char red, unsigned char green,
		unsigned char blue, size_t i, COLORREF color);
};

