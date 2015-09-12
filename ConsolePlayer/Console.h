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
#include<xstring>

 /*!
  * \class CConsole
  *
  * \brief 控制台类
  *
  * \author Mewiteor
  * \date 九月 2015
  */
class CConsole
{
public:
    //************************************
    // Method:      CConsole
    // FullName:    CConsole::CConsole
    // Access:      private 
    // Returns:     
    // Qualifier:  
    // Description: 构造控制台对象
    //************************************
    CConsole();

    //************************************
    // Method:      ~CConsole
    // FullName:    CConsole::~CConsole
    // Access:      private 
    // Returns:     
    // Qualifier:  
    // Description: 析构控制台对象
    //************************************
    ~CConsole();

    //************************************
    // Method:      WriteBg
    // FullName:    CConsole::WriteBg
    // Access:      private 
    // Returns:     DWORD
    // Qualifier:   const
    // Parameter:   const WORD * attr
    // Description: 写入全屏属性
    //************************************
    DWORD WriteBg(const WORD* attr) const;

    //************************************
    // Method:      GetSize
    // FullName:    CConsole::GetSize
    // Access:      private 
    // Returns:     const COORD&
    // Qualifier:   const
    // Description: 返回控制台当前大小
    //************************************
    const COORD& GetSize()const { return m_crSize; }

    //************************************
    // Method:      SetSize
    // FullName:    CConsole::SetSize
    // Access:      private 
    // Returns:     void
    // Qualifier:  
    // Parameter:   COORD cr
    // Description: 设置控制台大小
    //************************************
    void  SetSize(COORD cr);

    //************************************
    // Method:      SetColorTable
    // FullName:    CConsole::SetColorTable
    // Access:      private 
    // Returns:     void
    // Qualifier:   const
    // Parameter:   const COLORREF clrTb[16]
    // Description: 设置控制台的调色板
    //************************************
    void  SetColorTable(const COLORREF clrTb[16])  const;

    //************************************
    // Method:      SetTitle
    // FullName:    CConsole::SetTitle
    // Access:      private static 
    // Returns:     int
    // Qualifier:  
    // Parameter:   const char * format
    // Parameter:   ...
    // Description: 设置格式化的控制台标题
    //************************************
    static int SetTitle(const char* format, ...);

private:

    //************************************
    // Method:      GetMaxHeight
    // FullName:    CConsole::GetMaxHeight
    // Access:      private 
    // Returns:     void
    // Qualifier:  
    // Description: 获得控制台一次可写入的最大高度
    //************************************
    void GetMaxHeight();

public:
    static const COORD m_DEFAULT_FONT_COORD; // 控制台字体大小

private:
    HANDLE m_hStdIn,                            // 标准输入流
        m_hStdOut,                              // 标准输出流
        m_hNewOut;                              // 新创建的输出流
    CONSOLE_SCREEN_BUFFER_INFOEX m_oldCsbiex;   // 旧的控制台屏幕流信息的备份
    CONSOLE_FONT_INFOEX m_oldCfiex;             // 旧的控制台字体信息的备份
    DWORD m_dwOldShowStatus;                    // 旧的控制台显示模式的备份
    bool m_bSetCfi,                             // 是否设置了新的控制台字体信息
        m_bSetCsbi;                             // 是否设置了新的控制台屏幕流信息
    COORD m_crSize;                             // 控制台当前屏幕流大小
    DWORD m_dwLength;                           // 控制台屏幕流的总长度(m_crSize.X*m_crSize.Y)
    SHORT m_nMaxHeight;                         // 控制台一次可写入的最大高度
};

