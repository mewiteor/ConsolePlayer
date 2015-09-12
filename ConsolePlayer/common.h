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
  * \enum ColorType
  *
  * \brief 颜色模式枚举
  *
  * \author Mewiteor
  * \date 九月 2015
  */
enum ColorType
{
    MONO = 1002,	// 黑白，16级灰度
    STANDARD,	// 标准，使用控制台默认调色板
    AUTO		// 自动，实时计算帧的最优调色板
};

#define MAX_AUDIO_FRAME_SIZE 192000

// 格式化输出调试信息到输出窗口
#ifdef _DEBUG
#define DebugPrint(fmt,...) _DebugPrint(fmt,__VA_ARGS__)
#else
#define DebugPrint(fmt,...)
#endif

//************************************
// Method:      _DebugPrint
// FullName:    _DebugPrint
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   const char * format
// Parameter:   ...
// Description: 格式化输出调试信息到输出窗口
//************************************
void _DebugPrint(const char* format, ...);
