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
#include <xstring>
#include <cerrno>
#include <Strsafe.h>

 // 抛出Win32异常
#define Win32Throw(__fun__) throw CException(__FILE__,__LINE__,#__fun__,GetLastError())

// 抛出指定Win32异常
#define ThrowWin32Error(__fun__,__err__) throw CException(__FILE__,__LINE__,#__fun__,DWORD(__err__))

// 抛出指定C标准错误异常
#define ThrowCError(__fun__,__err__) throw CException(__FILE__,__LINE__,#__fun__,errno_t(__err__))

// 抛出指定文本信息错误异常
#define ThrowMsgError(__msg__) throw CException(__FILE__,__LINE__,nullptr,reinterpret_cast<const char*>(__msg__))

// 抛出指定文本信息错误异常
#define ThrowFunMsgError(__fun__,__msg__) throw CException(__FILE__,__LINE__,#__fun__,reinterpret_cast<const char*>(__msg__))

// 测试Win32API,若未调用成功,则抛出异常
#define TestWin32Throw(__fun__) do{\
        DWORD __dw__;\
        if(!(__fun__)&&(__dw__=GetLastError())) \
            throw CException(__FILE__,__LINE__,#__fun__,__dw__); \
    }while(0)

// 测试Win32 COM API,若未调用成功,则抛出异常
#define ComThrow(__fun__) do{\
        HRESULT __hr__ = __fun__; \
        if(FAILED(__hr__)) \
            throw CException(__FILE__,__LINE__,#__fun__,(DWORD)__hr__); \
    }while(0)

// 测试WaveOut API,若未调用成功,则抛出异常
#define WaveOutThrow(__fun__) do{\
    MMRESULT __mmResult__ = __fun__; \
    if(MMSYSERR_NOERROR!=__mmResult__) \
        throw CException(__FILE__,__LINE__,#__fun__,__mmResult__); \
    }while(0)

/*!
 * \class CException
 *
 * \brief Win32与C标准错误的异常类
 *
 * \author Mewiteor
 * \date 九月 2015
 */
class CException
{
public:

    //************************************
    // Method:      CException
    // FullName:    CException::CException
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   const char * lpFile
    // Parameter:   const size_t szLine
    // Parameter:   const char * strFunction
    // Parameter:   const DWORD dwLastError
    // Description: 构造Win32异常对象
    //************************************
    CException(const char* lpFile, const size_t szLine, const char* strFunction, const DWORD dwLastError);

    //************************************
    // Method:      CException
    // FullName:    CException::CException
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   const char * lpFile
    // Parameter:   const size_t szLine
    // Parameter:   const char * strFunction
    // Parameter:   const errno_t error
    // Description: 构造C标准异常对象
    //************************************
    CException(const char* lpFile, const size_t szLine, const char* strFunction, const errno_t error);

    //************************************
    // Method:      CException
    // FullName:    CException::CException
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   const char * lpFile
    // Parameter:   const size_t szLine
    // Parameter:   const char * strFunction
    // Parameter:   const MMRESULT error
    // Description: 构造waveOut异常对象
    //************************************
    CException(const char* lpFile, const size_t szLine, const char* strFunction, const MMRESULT error);

    //************************************
    // Method:      CException
    // FullName:    CException::CException
    // Access:      public 
    // Returns:     
    // Qualifier:  
    // Parameter:   const char * lpFile
    // Parameter:   const size_t szLine
    // Parameter:   const char * strFunction
    // Parameter:   const char * msg
    // Description: 构造文本信息异常对象
    //************************************
    CException(const char* lpFile, const size_t szLine, const char* strFunction, const char* msg);

    //************************************
    // Method:      ~CException
    // FullName:    CException::~CException
    // Access:      virtual public 
    // Returns:     
    // Qualifier:  
    // Description: 
    //************************************
    ~CException() {};

    //************************************
    // Method:      what
    // FullName:    CException::what
    // Access:      public 
    // Returns:     std::string
    // Qualifier:   const
    // Description: 返回异常字符串
    //************************************
    std::string what()const { return m_strErrorMessage; }

private:
    std::string m_strErrorMessage; // 储存异常字符串
};

