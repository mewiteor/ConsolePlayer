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
#include "Audio.h"
#include "Exception.h"


#define CLOSE_DEBUG_PRINT
#ifdef CLOSE_DEBUG_PRINT
#undef DebugPrint
#define DebugPrint(fmt,...) 
#endif

CAudio::DATA CAudio::m_cData[MAX_WAVE_DATA_COUNT];

CAudio::CAudio(WORD channels, DWORD sample_rate, WORD bits_per_sample,
    bool &_audioStart, bool &_videoStart, HANDLE &_hStartEvent)
    : m_hWaveOut(NULL)
    , m_dwStart(0)
    , m_dwCur(0)
    , m_dwEnd(0)
    , m_dwWaveHdrIndex(0)
    , m_dwCount(0)
    , m_hDataEvent(NULL)
    , m_hNotFullEvent(NULL)
    , m_hNotEmptyEvent(NULL)
    , m_hThread(NULL)
    , m_bContinue(FALSE)
    , m_bForce(FALSE)
    , m_bAudioStart(_audioStart)
    , m_bVideoStart(_videoStart)
    , m_hStartEvent(_hStartEvent)
{
    if (!_hStartEvent)ThrowWin32Error(CAudio::CVideo, ERROR_BAD_ARGUMENTS);
    MMRESULT mmResult;
    WAVEFORMATEX wfex = { WAVE_FORMAT_PCM,channels,sample_rate };
    wfex.wBitsPerSample = bits_per_sample;
    wfex.cbSize = sizeof(WAVEFORMATEX);
    wfex.nBlockAlign = (wfex.wBitsPerSample >> 3)*wfex.nChannels;
    wfex.nAvgBytesPerSec = wfex.nBlockAlign*wfex.nSamplesPerSec;

    TestWin32Throw(m_hDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL));
    TestWin32Throw(m_hNotFullEvent = CreateEvent(NULL, FALSE, FALSE, NULL));
    TestWin32Throw(m_hNotEmptyEvent = CreateEvent(NULL, FALSE, FALSE, NULL));
    mmResult = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &wfex, (DWORD_PTR)waveOutProc, (DWORD_PTR)m_hDataEvent, CALLBACK_FUNCTION);
    if (MMSYSERR_NOERROR != mmResult)throw mmResult;
    ZeroMemory(m_cWaveHdr, sizeof m_cWaveHdr);
    for (int i = 0; i < MAX_WAVE_COUNT; ++i)
        m_cWaveHdr[i].dwUser = (DWORD_PTR)&m_dwCount;
    m_bContinue = TRUE;
    m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
}

CAudio::~CAudio()
{
    if (m_hThread)
    {
        m_bContinue = FALSE;
        WaitForSingleObject(m_hThread, INFINITE);
    }
    if (m_hWaveOut)
        waveOutClose(m_hWaveOut);
    if (m_hDataEvent)
        CloseHandle(m_hDataEvent);
    if (m_hNotFullEvent)
        CloseHandle(m_hNotFullEvent);
    if (m_hNotEmptyEvent)
        CloseHandle(m_hNotEmptyEvent);
}

void CAudio::Push(unsigned char *buf, size_t len)
{
    while (DataFull())
    {
        DebugPrint("满\n");
        WaitForSingleObject(m_hNotFullEvent, INFINITE);
    }
    memcpy(m_cData[m_dwStart].data, buf, m_cData[m_dwStart].len = len);
    if (MAX_WAVE_DATA_COUNT == ++m_dwStart)m_dwStart = 0;
    DebugPrint("增加:%u:%u:%u\n", m_dwStart, m_dwCur, m_dwEnd);
    SetEvent(m_hNotEmptyEvent);
}

void CAudio::Stop(bool force)
{
    m_bContinue = FALSE;
    if (force)
        m_bForce = TRUE;
}

DWORD CALLBACK CAudio::ThreadProc(LPVOID lp)
{
    if (nullptr == lp)ThrowWin32Error(CAudio::ThreadProc, ERROR_BAD_ARGUMENTS);
    CAudio * audio = static_cast<CAudio*>(lp);
    return audio->AudioThreadProc();
}

DWORD CAudio::AudioThreadProc()
{
    for (;;)
    {
        while (m_bContinue&&DataEmpty())
        {
            DebugPrint("空\n");
            WaitForSingleObject(m_hNotEmptyEvent, 100);
        }
        if (m_bForce || !m_bContinue && DataEmpty())break;
        DebugPrint("m_cWaveHdr:index:%u,flags:%u\n", m_dwWaveHdrIndex, m_cWaveHdr[m_dwWaveHdrIndex].dwFlags);
        if (m_cWaveHdr[m_dwWaveHdrIndex].dwFlags & WHDR_PREPARED)
        {
            waveOutUnprepareHeader(m_hWaveOut, m_cWaveHdr + m_dwWaveHdrIndex, sizeof(WAVEHDR));
            if (MAX_WAVE_DATA_COUNT == ++m_dwEnd)m_dwEnd = 0;
            DebugPrint("减小:%u:%u:%u\n", m_dwStart, m_dwCur, m_dwEnd);
            SetEvent(m_hNotFullEvent);
        }
        m_cWaveHdr[m_dwWaveHdrIndex].lpData = m_cData[m_dwCur].data;
        m_cWaveHdr[m_dwWaveHdrIndex].dwBufferLength = (DWORD)m_cData[m_dwCur].len;
        if (!m_bAudioStart)
        {
            m_bAudioStart = true;
            SetEvent(m_hStartEvent);
            while (!m_bVideoStart)
                WaitForSingleObject(m_hStartEvent, 100);
        }
        waveOutPrepareHeader(m_hWaveOut, m_cWaveHdr + m_dwWaveHdrIndex, sizeof(WAVEHDR));
        waveOutWrite(m_hWaveOut, m_cWaveHdr + m_dwWaveHdrIndex, sizeof(WAVEHDR));
        if (MAX_WAVE_DATA_COUNT == ++m_dwCur)m_dwCur = 0;
        DebugPrint("中间:%u:%u:%u\n", m_dwStart, m_dwCur, m_dwEnd);
        if (MAX_WAVE_COUNT == ++m_dwWaveHdrIndex)m_dwWaveHdrIndex = 0;
        InterlockedIncrement(&m_dwCount);
        while (m_dwCount >= MAX_WAVE_COUNT)WaitForSingleObject(m_hDataEvent, INFINITE);
        if (MAX_WAVE_COUNT - 1 != m_dwCount)DebugPrint("%u\n", m_dwCount);
    }
    while (m_dwCount)WaitForSingleObject(m_hDataEvent, INFINITE);
    return 0;
}

void CALLBACK CAudio::waveOutProc(HWAVEOUT hWaveOut, UINT uMsg,
    DWORD_PTR dwInstance, // m_hDataEvent
    DWORD_PTR dwParam1, // m_cWaveHdr
    DWORD_PTR dwParam2)
{
    if (WOM_DONE == uMsg)
    {
        InterlockedDecrement(reinterpret_cast<LPDWORD>
            ((reinterpret_cast<LPWAVEHDR>(dwParam1))->dwUser /* &m_dwCount:当前正在播放的wave的数量*/));
        SetEvent(reinterpret_cast<HANDLE>(dwInstance));
    }
}
