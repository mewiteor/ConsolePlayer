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
#include "Console.h"
#include<thread>
#include<cstdarg>
#include"Exception.h"

const COORD CConsole::m_DEFAULT_FONT_COORD = COORD{ 3,5 };
using namespace std;

CConsole::CConsole()
    : m_hStdIn(INVALID_HANDLE_VALUE)
    , m_hStdOut(INVALID_HANDLE_VALUE)
    , m_hNewOut(INVALID_HANDLE_VALUE)
    , m_oldCfiex({ sizeof(CONSOLE_FONT_INFOEX) })
    , m_oldCsbiex({ sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) })
    , m_bSetCfi(false)
    , m_bSetCsbi(false)
    , m_dwOldShowStatus(-1)
    , m_dwLength(0)
{
    m_hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (INVALID_HANDLE_VALUE == m_hStdIn)Win32Throw(GetStdHandle);
    m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (INVALID_HANDLE_VALUE == m_hStdOut)Win32Throw(GetStdHandle);
    m_hNewOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
    if (INVALID_HANDLE_VALUE == m_hNewOut)Win32Throw(CreateConsoleScreenBuffer);
    TestWin32Throw(SetConsoleActiveScreenBuffer(m_hNewOut));
    // 最小的点阵字体
    CONSOLE_FONT_INFOEX cfiex =
    {
        sizeof(CONSOLE_FONT_INFOEX),
        0,
        m_DEFAULT_FONT_COORD,
        0,
        FW_NORMAL,
        L"Terminal"
    };
    TestWin32Throw(GetCurrentConsoleFontEx(m_hNewOut, FALSE, &m_oldCfiex));
    TestWin32Throw(SetCurrentConsoleFontEx(m_hNewOut, FALSE, &cfiex));
    m_bSetCfi = true;
    COORD max = GetLargestConsoleWindowSize(m_hNewOut);
    WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
    TestWin32Throw(GetWindowPlacement(GetConsoleWindow(), &wp));
    m_dwOldShowStatus = wp.showCmd;
    if (wp.showCmd != SW_MAXIMIZE)
        ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);                    // 最大化
    CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
    TestWin32Throw(GetConsoleScreenBufferInfoEx(m_hNewOut, &csbiex));
    TestWin32Throw(memcpy(&m_oldCsbiex, &csbiex, sizeof csbiex));
    csbiex.dwCursorPosition = COORD{ 0,0 };
    csbiex.bFullscreenSupported = FALSE;
    m_crSize = csbiex.dwSize = max;
    m_dwLength = m_crSize.X*m_crSize.Y;
    csbiex.srWindow = SMALL_RECT{ 0,0,csbiex.dwSize.X,csbiex.dwSize.Y };
    TestWin32Throw(SetConsoleScreenBufferInfoEx(m_hNewOut, &csbiex));   // 设为最大的大小
    m_bSetCsbi = true;
    CONSOLE_CURSOR_INFO cci = { sizeof(CONSOLE_CURSOR_INFO) };
    TestWin32Throw(SetConsoleCursorInfo(m_hNewOut, &cci));              // 隐藏光标
    GetMaxHeight();
}

CConsole::~CConsole()
{
    if (INVALID_HANDLE_VALUE != m_hNewOut)
    {
        if (m_dwOldShowStatus != -1)
            ShowWindow(GetConsoleWindow(), m_dwOldShowStatus);
        if (m_bSetCsbi)
            SetConsoleScreenBufferInfoEx(m_hNewOut, &m_oldCsbiex);
        if (m_bSetCfi)
            SetCurrentConsoleFontEx(m_hNewOut, FALSE, &m_oldCfiex);
        if (INVALID_HANDLE_VALUE != m_hStdOut)
            SetConsoleActiveScreenBuffer(m_hStdOut);
        CloseHandle(m_hNewOut);
    }
}

DWORD CConsole::WriteBg(const WORD* attr)  const
{
    DWORD _dw;
    if(m_crSize.Y == m_nMaxHeight)
        TestWin32Throw(WriteConsoleOutputAttribute(m_hNewOut, attr, m_dwLength, COORD{ 0,0 }, &_dw));
    else
    {
        size_t step = m_crSize.X*m_nMaxHeight;
        short n = m_crSize.Y / m_nMaxHeight, r = m_crSize.Y %m_nMaxHeight;
        COORD cr = { 0 };
        for (short i = 0; i < n; ++i, attr += step, cr.Y += m_nMaxHeight)
            TestWin32Throw(WriteConsoleOutputAttribute(m_hNewOut, attr, DWORD(step), cr, &_dw));
        if (r)
            TestWin32Throw(WriteConsoleOutputAttribute(m_hNewOut, attr, r*m_crSize.X, cr, &_dw));
    }
    return _dw;
}

void CConsole::SetSize(COORD cr)
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
    TestWin32Throw(GetConsoleScreenBufferInfoEx(m_hNewOut, &csbiex));
    m_crSize = csbiex.dwSize = cr;
    m_dwLength = m_crSize.X*m_crSize.Y;
    csbiex.srWindow = { 0,0,m_crSize.X - 1,m_crSize.Y - 1 };
    TestWin32Throw(SetConsoleScreenBufferInfoEx(m_hNewOut, &csbiex));
    TestWin32Throw(SetConsoleWindowInfo(m_hNewOut, TRUE, &csbiex.srWindow));
    GetMaxHeight();
}
void CConsole::SetColorTable(const COLORREF clrTb[16])  const
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
    TestWin32Throw(GetConsoleScreenBufferInfoEx(m_hNewOut, &csbiex));
    memcpy(csbiex.ColorTable, clrTb, sizeof csbiex.ColorTable);
    TestWin32Throw(SetConsoleScreenBufferInfoEx(m_hNewOut, &csbiex));
}

int CConsole::SetTitle(const char * format, ...)
{
    char str[MAX_PATH + 256];
    va_list va;
    va_start(va, format);
    int r = vsprintf_s(str, format, va);
    va_end(va);
    TestWin32Throw(SetConsoleTitle(str));
    return r;
}

void CConsole::GetMaxHeight()
{
    DWORD _dw;
    DWORD err = 0;
    WORD *ws = new WORD[m_crSize.X*m_crSize.Y]();
    m_nMaxHeight = 0;
    SetLastError(0);
    while (m_nMaxHeight < m_crSize.Y&&
        WriteConsoleOutputAttribute(m_hNewOut, ws,
            (m_nMaxHeight + 1) * m_crSize.X, COORD{ 0,0 }, &_dw))
        ++m_nMaxHeight;
    if (m_nMaxHeight < m_crSize.Y)
        err = GetLastError();
    delete[] ws;
    if (err&&ERROR_NOT_ENOUGH_MEMORY != err)
        ThrowWin32Error(WriteConsoleOutputAttribute, err);
}
