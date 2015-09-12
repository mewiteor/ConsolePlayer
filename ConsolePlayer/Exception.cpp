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
#include <windows.h>
#include "Exception.h"
#include <sstream>

using namespace std;

CException::CException(const char* lpFile, const size_t szLine, const char* strFunction, const DWORD dwLastError)
{
    char* lpMsgBuf = nullptr;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf,
        0, NULL);
    ostringstream oss;
    oss << "错误文件：" << lpFile
        << "\n错误行号：" << szLine
        << "\n错误函数：" << strFunction
        << "\n错误码：" << dwLastError
        << "\n错误信息：" << lpMsgBuf;
    m_strErrorMessage = oss.str();
    LocalFree(lpMsgBuf);
}

CException::CException(const char * lpFile, const size_t szLine, const char * strFunction, const errno_t error)
{
    ostringstream oss;
    char lpErrMsg[128];
    strerror_s(lpErrMsg, error);
    oss << "错误文件：" << lpFile
        << "\n错误行号：" << szLine
        << "\n错误函数：" << strFunction
        << "\n错误码：" << error
        << "\n错误信息：" << lpErrMsg;
    m_strErrorMessage = oss.str();
}

CException::CException(const char * lpFile, const size_t szLine, const char * strFunction, const MMRESULT error)
{
    ostringstream oss;
    char lpErrMsg[256];
    waveOutGetErrorTextA(error, lpErrMsg, _countof(lpErrMsg));
    oss << "错误文件：" << lpFile
        << "\n错误行号：" << szLine
        << "\n错误函数：" << strFunction
        << "\n错误码：" << error
        << "\n错误信息：" << lpErrMsg;
    m_strErrorMessage = oss.str();
}

CException::CException(const char* lpFile, const size_t szLine, const char* strFunction, const char* msg)
{
    ostringstream oss;
    oss << "错误文件：" << lpFile
        << "\n错误行号：" << szLine;
    if (strFunction)
        oss << "\n错误函数：" << strFunction;
    oss << "\n错误信息：" << msg;
    m_strErrorMessage = oss.str();
}
