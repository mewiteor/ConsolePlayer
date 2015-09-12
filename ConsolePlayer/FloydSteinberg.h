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
  * \class CFloydSteinberg
  *
  * \brief Floyd Steinberg算法
  *
  * \author Mewiteor
  * \date 九月 2015
  */
class CFloydSteinberg :
    public CErrorDiffusion
{
public:

    //************************************
    // Method:      CFloydSteinberg
    // FullName:    CFloydSteinberg::CFloydSteinberg
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   unsigned int width
    // Parameter:   unsigned int height
    // Description: 
    //************************************
    CFloydSteinberg(unsigned int width, unsigned int height);

    //************************************
    // Method:      ~CFloydSteinberg
    // FullName:    CFloydSteinberg::~CFloydSteinberg
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Description: 
    //************************************
    ~CFloydSteinberg();

    //************************************
    // Method:      Diffusion
    // FullName:    CFloydSteinberg::Diffusion
    // Access:      public 
    // Returns:     void
    // Qualifier:  
    // Parameter:   unsigned char red
    // Parameter:   unsigned char green
    // Parameter:   unsigned char blue
    // Parameter:   size_t i
    // Parameter:   COLORREF color
    // Description: 误差扩散
    //                  | _ * 7 |     
    //                  | 3 5 1 |*1/16
    //************************************
    void Diffusion(unsigned char red, unsigned char green,
        unsigned char blue, size_t i, COLORREF color);
};

