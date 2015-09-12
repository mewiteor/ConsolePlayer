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
#include "Dither.h"
#include<windows.h>

typedef double _REAL;

/*!
 * \class CErrorDiffusion
 *
 * \brief 误差扩散类
 *
 * \author Mewiteor
 * \date 九月 2015
 */
class CErrorDiffusion :
	public CDither
{
public:
	struct COLOR	// 颜色结构
	{
		_REAL red;		// 红色累积误差
		_REAL green;	// 绿色累积误差
		_REAL blue;	// 蓝色累积误差

		//************************************
		// Method:      COLOR
		// FullName:    CErrorDiffusion::COLOR::COLOR
		// Access:      public 
		// Returns:     
		// Qualifier:  
		// Description: 
		//************************************
		COLOR() {}

		//************************************
		// Method:      COLOR
		// FullName:    CErrorDiffusion::COLOR::COLOR
		// Access:      public 
		// Returns:     
		// Qualifier:  
		// Parameter:   const COLORREF & color
		// Description: 
		//************************************
		COLOR(const COLORREF& color);

		//************************************
		// Method:      operator+=
		// FullName:    CErrorDiffusion::COLOR::operator+=
		// Access:      public 
		// Returns:     COLOR&
		// Qualifier:  
		// Parameter:   const COLOR & color
		// Description: 
		//************************************
		COLOR& operator +=(const COLOR &color);

		//************************************
		// Method:      operator-=
		// FullName:    CErrorDiffusion::COLOR::operator-=
		// Access:      public 
		// Returns:     COLOR&
		// Qualifier:  
		// Parameter:   const COLOR & color
		// Description: 
		//************************************
		COLOR& operator -=(const COLOR &color);

		//************************************
		// Method:      operator+
		// FullName:    CErrorDiffusion::COLOR::operator+
		// Access:      public 
		// Returns:     CErrorDiffusion::COLOR
		// Qualifier:   const
		// Parameter:   const COLOR & color
		// Description: 
		//************************************
		COLOR operator +(const COLOR &color)const;

		//************************************
		// Method:      operator-
		// FullName:    CErrorDiffusion::COLOR::operator-
		// Access:      public 
		// Returns:     CErrorDiffusion::COLOR
		// Qualifier:   const
		// Parameter:   const COLOR & color
		// Description: 
		//************************************
		COLOR operator -(const COLOR &color)const;

		//************************************
		// Method:      operator+=
		// FullName:    CErrorDiffusion::COLOR::operator+=
		// Access:      public 
		// Returns:     COLOR&
		// Qualifier:  
		// Parameter:   const _REAL & num
		// Description: 
		//************************************
		COLOR& operator +=(const _REAL &num);

		//************************************
		// Method:      operator+
		// FullName:    CErrorDiffusion::COLOR::operator+
		// Access:      public 
		// Returns:     COLOR&
		// Qualifier:  
		// Parameter:   const _REAL & num
		// Description: 
		//************************************
		COLOR operator +(const _REAL &num)const;

		//************************************
		// Method:      operator*=
		// FullName:    CErrorDiffusion::COLOR::operator*=
		// Access:      public 
		// Returns:     COLOR&
		// Qualifier:  
		// Parameter:   const _REAL & num
		// Description: 
		//************************************
		COLOR& operator *=(const _REAL &num);

		//************************************
		// Method:      operator/=
		// FullName:    CErrorDiffusion::COLOR::operator/=
		// Access:      public 
		// Returns:     COLOR&
		// Qualifier:  
		// Parameter:   const _REAL & num
		// Description: 
		//************************************
		COLOR& operator /=(const _REAL &num);

		//************************************
		// Method:      operator*
		// FullName:    CErrorDiffusion::COLOR::operator*
		// Access:      public 
		// Returns:     CErrorDiffusion::COLOR
		// Qualifier:   const
		// Parameter:   const _REAL & num
		// Description: 
		//************************************
		COLOR operator *(const _REAL &num)const;

		//************************************
		// Method:      operator/
		// FullName:    CErrorDiffusion::COLOR::operator/
		// Access:      public 
		// Returns:     CErrorDiffusion::COLOR
		// Qualifier:   const
		// Parameter:   const _REAL & num
		// Description: 
		//************************************
		COLOR operator /(const _REAL &num)const;
	};
public:

	//************************************
	// Method:      CErrorDiffusion
	// FullName:    CErrorDiffusion::CErrorDiffusion
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   unsigned int width
	// Parameter:   unsigned int height
	// Description: 
	//************************************
	CErrorDiffusion(unsigned int width, unsigned int height);

	//************************************
	// Method:      ~CErrorDiffusion
	// FullName:    CErrorDiffusion::~CErrorDiffusion
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Description: 
	//************************************
	~CErrorDiffusion();
	
	//************************************
	// Method:      Init
	// FullName:    CErrorDiffusion::Init
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Description: 初始化
	//************************************
	void Init();

	//************************************
	// Method:      Get
	// FullName:    CErrorDiffusion::Get
	// Access:      public 
	// Returns:     COLORREF
	// Qualifier:  
	// Parameter:   unsigned char red
	// Parameter:   unsigned char green
	// Parameter:   unsigned char blue
	// Parameter:   size_t i
	// Description: 返回加上累积误差后的颜色
	//************************************
	COLORREF Get(unsigned char red, unsigned char green,
		unsigned char blue, size_t i);
	
	//************************************
	// Method:      Diffusion
	// FullName:    CErrorDiffusion::Diffusion
	// Access:      virtual public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   unsigned char red
	// Parameter:   unsigned char green
	// Parameter:   unsigned char blue
	// Parameter:   size_t i
	// Parameter:   COLORREF color
	// Description: 误差扩散
	//************************************
	virtual void  Diffusion(unsigned char red, unsigned char green,
		unsigned char blue, size_t i, COLORREF color) = 0 ;
protected:
	COLOR *m_clrErrors;	// 累积误差
};

