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
#include "ErrorDiffusion.h"
 /*!
  * \class CStucki
  *
  * \brief Stucki算法
  *
  * \author Mewiteor
  * \date 九月 2015
  */
class CStucki :
    public CErrorDiffusion
{
public:

    //************************************
    // Method:      CStucki
    // FullName:    CStucki::CStucki
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   unsigned int width
    // Parameter:   unsigned int height
    // Description: 
    //************************************
    CStucki(unsigned int width, unsigned int height);

    //************************************
    // Method:      ~CStucki
    // FullName:    CStucki::~CStucki
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Description: 
    //************************************
    ~CStucki();

    //************************************
    // Method:      Diffusion
    // FullName:    CStucki::Diffusion
    // Access:      public 
    // Returns:     void
    // Qualifier:  
    // Parameter:   unsigned char red
    // Parameter:   unsigned char green
    // Parameter:   unsigned char blue
    // Parameter:   size_t i
    // Parameter:   COLORREF color
    // Description: 误差扩散
    //                  | ___ * 8 4 |
    //                  | 2 4 8 4 2 |*1/42
    //                  | 1 2 4 2 1 |
    //************************************
    void Diffusion(unsigned char red, unsigned char green,
        unsigned char blue, size_t i, COLORREF color);
};

