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
 /*!
  * \class CBayer
  *
  * \brief Bayer抖动类
  *
  * \author Mewiteor
  * \date 九月 2015
  */
class CBayer :
    public CDither
{
public:

    //************************************
    // Method:      CBayer
    // FullName:    CBayer::CBayer
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   unsigned int width
    // Parameter:   unsigned int height
    // Description: 构造Bayer抖动对象
    //************************************
    CBayer(unsigned int width, unsigned int height);

    //************************************
    // Method:      ~CBayer
    // FullName:    CBayer::~CBayer
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Description: 析构Bayer抖动对象
    //************************************
    ~CBayer();

    //************************************
    // Method:      Get
    // FullName:    CBayer::Get
    // Access:      public 
    // Returns:     unsigned char
    // Qualifier:  
    // Parameter:   unsigned char alpha,256级灰度值
    // Parameter:   size_t i,像素在图像中的索引
    // Description: 返回Bayer抖动后的16级灰度值
    //************************************
    unsigned char Get(unsigned char alpha, size_t i);
private:
    static const unsigned char m_nM[16];    // Bayer抖动表
};

