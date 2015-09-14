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
#include "Video.h"
#include "ColorQuantization.h"
#include "FloydSteinberg.h"
#include "MyErrorDiffusion.h"
#include "Stucki.h"
#include "Bayer.h"
#include "Exception.h"

#define NONO_MODE 0
#define FLOYD_STEINBERG_MODE 1
#define STUCKI_MODE 2
#define MY_ERROR_DIFFUSION_MODE 3
#define CUR_MODE MY_ERROR_DIFFUSION_MODE

#define CLOSE_DEBUG_PRINT
#ifdef CLOSE_DEBUG_PRINT
#undef DebugPrint
#define DebugPrint(fmt,...) 
#endif


CVideo::CVideo(const CConsole &console, DWORD m_dwDuration,
    ColorType colorType, bool &audioStart, bool &videoStart,
    HANDLE m_hStartEvent, const char* filePath,
    COLORREF defaultColorTable[16] /*= nullptr*/)
    : m_cConsole(console)
    , m_cColorType(colorType)
    , m_dwDuration(m_dwDuration)
    , m_nFrames(0)
    , m_dwStartTime(0)
    , m_dwLastTime(0)
    , m_dwLastTimeSeconds(0)
    , m_fLastFps(0.0f)
    , m_pColorQuantization(nullptr)
    , m_pErrorDiffusion(nullptr)
    , m_pBayer(nullptr)
    , m_nLength(0)
    , m_bContinue(false)
    , m_bForce(false)
    , m_nDataStart(0)
    , m_nDataToAttrIndex(0)
    , m_nAttrEnd(0)
    , m_bAudioStart(audioStart)
    , m_bVideoStart(videoStart)
    , m_hStartEvent(m_hStartEvent)
    , m_strPlayFilePath(filePath)
    , m_pDefaultColorTable(defaultColorTable)
    , m_hDataNotEmptyEvent(nullptr)
    , m_hAttrNotFullEvent(nullptr)
    , m_hConvertDataToAttrEvent(nullptr)
    , m_dwPeriodMin(0)
{
    if (!m_dwDuration || !m_hStartEvent || (ColorType::AUTO != colorType&&nullptr == defaultColorTable))ThrowWin32Error(CVideo::CVideo, ERROR_BAD_ARGUMENTS);
    m_nLength = (size_t)console.GetSize().X*console.GetSize().Y;
#if CUR_MODE==FLOYD_STEINBERG_MODE
    m_pErrorDiffusion = new CFloydSteinberg(console.GetSize().X, console.GetSize().Y);
#elif CUR_MODE==STUCKI_MODE
    m_pErrorDiffusion = new CStucki(console.GetSize().X, console.GetSize().Y);
#elif CUR_MODE==MY_ERROR_DIFFUSION_MODE
    m_pErrorDiffusion = new CMyErrorDiffusion(console.GetSize().X, console.GetSize().Y);
#elif CUR_MODE!=NONO_MODE
#error "未知的CUR_MODE值"
#endif
    if (m_pErrorDiffusion)m_pErrorDiffusion->Init();
    m_pBayer = new CBayer(console.GetSize().X, console.GetSize().Y);
    if (colorType == ColorType::AUTO)
        m_pColorQuantization = new CColorQuantization(16);
    for (auto &p : m_cData)
        p.data = new uint8_t[3 * m_nLength];
    for (auto& p : m_cAttributes)
        p.attr = new WORD[m_nLength];
    m_hDataNotEmptyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    m_hConvertDataToAttrEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    m_hAttrNotFullEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    TIMECAPS timeCaps;
    timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));
    m_dwPeriodMin = timeCaps.wPeriodMin;
    timeBeginPeriod(m_dwPeriodMin);
    m_bContinue = true;
    m_hColorQuantizationThread = CreateThread(NULL, 0, ColorQuantizationThread, this, 0, nullptr);
    m_hWriteAttrThread = CreateThread(NULL, 0, WriteAttrThread, this, 0, nullptr);
}

CVideo::~CVideo()
{
    WaitForSingleObject(m_hColorQuantizationThread, INFINITE);
    WaitForSingleObject(m_hWriteAttrThread, INFINITE);
    if (m_dwPeriodMin)
        timeEndPeriod(m_dwPeriodMin);
    if (m_hAttrNotFullEvent)
        CloseHandle(m_hAttrNotFullEvent);
    if (m_hConvertDataToAttrEvent)
        CloseHandle(m_hConvertDataToAttrEvent);
    if (m_hDataNotEmptyEvent)
        CloseHandle(m_hDataNotEmptyEvent);
    for (auto &p : m_cData)
        if (p.data)
            delete[] p.data;
    for (auto &p : m_cAttributes)
        if (p.attr)
            delete[] p.attr;
    if (m_pColorQuantization)
        delete m_pColorQuantization;
    if (m_pBayer)
        delete m_pBayer;
    if (m_pErrorDiffusion)
        delete m_pErrorDiffusion;
}

void CVideo::Push(unsigned char *buf, size_t len, int64_t timestamp)
{
    while (DataFull())
    {
        DebugPrint("数据满\n");
        WaitForSingleObject(m_hConvertDataToAttrEvent, INFINITE);
    }
    DebugPrint("%lld\n", timestamp);
    memcpy(m_cData[m_nDataStart].data, buf, len);
    m_cData[m_nDataStart].timestamp = timestamp;
    if (MAX_VIDEO_DATA_COUNT == ++m_nDataStart)m_nDataStart = 0;
    SetEvent(m_hDataNotEmptyEvent);
}

void CVideo::Stop(bool force)
{
    m_bContinue = FALSE;
    m_bForce = force;
}

DWORD CALLBACK CVideo::ColorQuantizationThread(LPVOID lp)
{
    if (nullptr == lp)ThrowWin32Error(CVideo::ColorQuantizationThread, ERROR_BAD_ARGUMENTS);
    CVideo* video = static_cast<CVideo*>(lp);
    return video->VideoColorQuantizationThread();
}

DWORD CVideo::VideoColorQuantizationThread()
{
    for (;;)
    {
        while (m_bContinue&&DataEmpty())
        {
            DebugPrint("数据空\n");
            WaitForSingleObject(m_hDataNotEmptyEvent, 100);
        }
        if (m_bForce || (!m_bContinue&&DataEmpty()))break;
        while (AttrFull())
        {
            DebugPrint("属性满\n");
            WaitForSingleObject(m_hAttrNotFullEvent, 100);
        }
        switch (m_cColorType)
        {
        case ColorType::STANDARD:
#if CUR_MODE!=NONO_MODE
            m_pErrorDiffusion->Init();
#endif
            for (size_t i = 0; i < m_nLength; ++i)
            {
                auto
                    red = m_cData[m_nDataToAttrIndex].data[i * 3],
                    green = m_cData[m_nDataToAttrIndex].data[i * 3 + 1],
                    blue = m_cData[m_nDataToAttrIndex].data[i * 3 + 2];
#if CUR_MODE!=NONO_MODE
                auto color = m_pErrorDiffusion->Get(red, green, blue, i);
                auto err_diff_red = GetRValue(color);
                auto err_diff_green = GetGValue(color);
                auto err_diff_blue = GetBValue(color);
                auto attr = Convert24bppTo4bpp(err_diff_red, err_diff_green, err_diff_blue);
#else
                auto attr = Convert24bppTo4bpp(red, green, blue);
#endif
                m_cAttributes[m_nDataToAttrIndex].attr[i] = attr;
#if CUR_MODE!=NONO_MODE
                m_pErrorDiffusion->Diffusion(red, green, blue, i, m_pDefaultColorTable[attr >> 4]);
#endif
            }
            break;
        case ColorType::MONO:
            for (size_t i = 0; i < m_nLength; ++i)
            {
                m_cAttributes[m_nDataToAttrIndex].attr[i] = m_pBayer->Get((1 +
                    m_cData[m_nDataToAttrIndex].data[i * 3] +
                    m_cData[m_nDataToAttrIndex].data[i * 3 + 1] +
                    m_cData[m_nDataToAttrIndex].data[i * 3 + 2]
                    ) / 3, i) & 0xf0;
            }
            break;
        case ColorType::AUTO:
            for (size_t i = 0; i < m_nLength; ++i)
                m_pColorQuantization->Insert(
                    m_cData[m_nDataToAttrIndex].data[i * 3],
                    m_cData[m_nDataToAttrIndex].data[i * 3 + 1],
                    m_cData[m_nDataToAttrIndex].data[i * 3 + 2]);
            m_pColorQuantization->Done(m_cAttributes[m_nDataToAttrIndex].colorTable);
#if CUR_MODE!=NONO_MODE
            m_pErrorDiffusion->Init();
#endif
            for (size_t i = 0; i < m_nLength; ++i)
            {
                auto
                    red = m_cData[m_nDataToAttrIndex].data[i * 3],
                    green = m_cData[m_nDataToAttrIndex].data[i * 3 + 1],
                    blue = m_cData[m_nDataToAttrIndex].data[i * 3 + 2];
#if CUR_MODE!=NONO_MODE
                auto color = m_pErrorDiffusion->Get(red, green, blue, i);

                auto err_diff_red = GetRValue(color);
                auto err_diff_green = GetGValue(color);
                auto err_diff_blue = GetBValue(color);
                auto attr = m_pColorQuantization->Get(err_diff_red, err_diff_green, err_diff_blue);
#else
                auto attr = m_pColorQuantization->Get(red, green, blue);

#endif
                m_cAttributes[m_nDataToAttrIndex].attr[i] = attr << 4 & 0xf0;
#if CUR_MODE!=NONO_MODE
                m_pErrorDiffusion->Diffusion(red, green, blue, i, m_cAttributes[m_nDataToAttrIndex].colorTable[attr]);
#endif
            }
            break;
        default:
            break;
        }
        m_cAttributes[m_nDataToAttrIndex].timestamp = m_cData[m_nDataToAttrIndex].timestamp;
        if (MAX_VIDEO_DATA_COUNT == ++m_nDataToAttrIndex)m_nDataToAttrIndex = 0;
        SetEvent(m_hConvertDataToAttrEvent);
    }
    return 0;
}

DWORD CALLBACK CVideo::WriteAttrThread(LPVOID lp)
{
    if (nullptr == lp)ThrowWin32Error(CVideo::WriteAttrThread, ERROR_BAD_ARGUMENTS);
    CVideo* video = static_cast<CVideo*>(lp);
    return video->VideoWriteAttrThread();
}

DWORD CVideo::VideoWriteAttrThread()
{
    for (;;)
    {
        while (m_bContinue&&AttrEmpty())
        {
            DebugPrint("属性空\n");
            WaitForSingleObject(m_hConvertDataToAttrEvent, 100);
        }
        if (m_bForce || (!m_bContinue&&AttrEmpty()))break;
        DWORD end, now = timeGetTime();
        if (!m_bVideoStart)
        {
            m_bVideoStart = true;
            SetEvent(m_hStartEvent);
            while (!m_bAudioStart)
                WaitForSingleObject(m_hStartEvent, 100);
            end = m_dwStartTime = now;
        }
        else
            end = DWORD(m_dwStartTime + m_cAttributes[m_nAttrEnd].timestamp);
        if (end >= now)
        {
            if (end > now)
                Sleep(end - now);
            bool bChange = false;
            auto curTimeSeconds = (DWORD)m_cAttributes[m_nAttrEnd].timestamp / 1000;
            DebugPrint("%lu:%lu:%lu\n", m_dwLastTimeSeconds, curTimeSeconds, m_dwDuration);
            if (m_dwLastTimeSeconds != curTimeSeconds)
            {
                m_dwLastTimeSeconds = (DWORD)curTimeSeconds;
                bChange = true;
            }
            if (!m_dwLastTime)
                m_dwLastTime = end;
            else
            {
                ++m_nFrames;
                auto diff = end - m_dwLastTime;
                if (diff >= 1000)
                {
                    m_fLastFps = m_nFrames*1000.0f / diff;
                    m_nFrames = 0;
                    bChange = true;
                    m_dwLastTime = end;
                }
            }
            if (bChange)
                CConsole::SetTitle("正在播放:%s [%02lu:%02lu:%02lu/%02lu:%02lu:%02lu][%06.3ffps]",
                    m_strPlayFilePath, m_dwLastTimeSeconds / 3600, m_dwLastTimeSeconds / 60 % 60, m_dwLastTimeSeconds % 60,
                    m_dwDuration / 3600, m_dwDuration / 60 % 60, m_dwDuration % 60, m_fLastFps);
            if (m_cColorType == ColorType::AUTO)
                m_cConsole.SetColorTable(m_cAttributes[m_nAttrEnd].colorTable);
            m_cConsole.WriteBg(m_cAttributes[m_nAttrEnd].attr);
        }
        if (MAX_VIDEO_DATA_COUNT == ++m_nAttrEnd)m_nAttrEnd = 0;
        SetEvent(m_hAttrNotFullEvent);
    }
    return 0;
}

WORD CVideo::Convert24bppTo4bpp(short red, short green, short blue)
{
    if (red + green + blue >= 0x2a0)return 0xf0;
    else if (red <= 0x40 && green <= 0x40 && blue <= 0x40)return 0x00;
    else if (red <= 0x40 && green <= 0x40 && blue >= 0x40 && blue <= 0xc0)return 0x10;
    else if (red <= 0x40 && green >= 0x40 && green <= 0xc0 && blue <= 0x40)return 0x20;
    else if (red >= 0x40 && red <= 0xc0 && green <= 0x40 && blue <= 0x40)return 0x40;
    else if (green + blue >= 0x180 && green + blue - red >= 0x140 && green + blue - 3 * red >= 0xA0)return 0xb0;
    else if (red + blue >= 0x180 && red + blue - green >= 0x140 && red + blue - 3 * green >= 0xA0)return 0xd0;
    else if (red + green >= 0x180 && red + green - blue >= 0x140 && red + green - 3 * blue >= 0xA0)return 0xe0;
    else if (blue >= 0xc0 && blue - red >= 0x80 && blue - green >= 0x80 && blue - red - green >= 0x40)return 0x90;
    else if (green >= 0xc0 && green - red >= 0x80 && green - blue >= 0x80 && green - red - blue >= 0x40)return 0xa0;
    else if (red >= 0xc0 && red - green >= 0x80 && red - blue >= 0x80 && red - green - blue >= 0x40)return 0xc0;
    else if (red <= 0x40 && green >= 0x40 && blue >= 0x40 && green - blue <= 0x80 && blue - green <= 0x80 && green + blue <= 0x180)return 0x30;
    else if (red >= 0x40 && green <= 0x40 && blue >= 0x40 && red - blue <= 0x80 && blue - red <= 0x80 && red + blue <= 0x180)return 0x50;
    else if (blue <= 0x40 && green >= 0x40 && red >= 0x40 && green - red <= 0x80 && red - green <= 0x80 && red + green <= 0x180)return 0x60;
    else if (red + green + blue >= 0x1e0 && red + green - 3 * blue <= 0xA0 && red + blue - 3 * green <= 0xA0 && green + blue - 3 * red <= 0xA0)return 0x70;
    else return 0x80;
}
