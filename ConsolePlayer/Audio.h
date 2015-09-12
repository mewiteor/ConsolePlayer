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
#include "common.h"
#include <wtypes.h>
/*!
 * \class CAudio
 *
 * \brief 音频播放类
 *
 * \author Mewiteor
 * \date 九月 2015
 */
class CAudio
{
	enum
	{
		MAX_WAVE_COUNT=8,
		MAX_WAVE_DATA_COUNT=64
	};
	struct DATA { char data[MAX_AUDIO_FRAME_SIZE]; size_t len; };
public:

	//************************************
	// Method:      CAudio
	// FullName:    CAudio::CAudio
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   WORD channels
	// Parameter:   DWORD sample_rate
	// Parameter:   WORD bits_per_sample
	// Parameter:   bool & _audioStart
	// Parameter:   bool & _videoStart
	// Parameter:   HANDLE & _hStartEvent
	// Description: 
	//************************************
	CAudio(WORD channels, DWORD sample_rate, WORD bits_per_sample,
		bool &_audioStart, bool &_videoStart,HANDLE &_hStartEvent);

	//************************************
	// Method:      ~CAudio
	// FullName:    CAudio::~CAudio
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Description: 
	//************************************
	~CAudio();

	//************************************
	// Method:      Push
	// FullName:    CAudio::Push
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   unsigned char * buf
	// Parameter:   size_t len
	// Description: 压入音频数据到data循环队列
	//************************************
	void Push(unsigned char *buf, size_t len);
	
	//************************************
	// Method:      Stop
	// FullName:    CAudio::Stop
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   bool force,true:按ESC退出,false:播放完退出
	// Description: 播放结束
	//************************************
	void Stop(bool force);
private:
	
	//************************************
	// Method:      ThreadProc
	// FullName:    CAudio::ThreadProc
	// Access:      private static 
	// Returns:     DWORD CALLBACK
	// Qualifier:  
	// Parameter:   LPVOID lp
	// Description: 
	//************************************
	static DWORD CALLBACK ThreadProc(LPVOID lp);

	//************************************
	// Method:      AudioThreadProc
	// FullName:    CAudio::AudioThreadProc
	// Access:      private 
	// Returns:     DWORD
	// Qualifier:  
	// Description: 
	//************************************
	DWORD AudioThreadProc();
	
	//************************************
	// Method:      waveOutProc
	// FullName:    CAudio::waveOutProc
	// Access:      private static 
	// Returns:     void CALLBACK
	// Qualifier:  
	// Parameter:   HWAVEOUT hWaveOut
	// Parameter:   UINT uMsg
	// Parameter:   DWORD_PTR dwInstance,hDataEvent
	// Parameter:   DWORD_PTR dwParam1,waveHdr
	// Parameter:   DWORD_PTR dwParam2
	// Description: 
	//************************************
	static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance,
		DWORD_PTR dwParam1, DWORD_PTR dwParam2);

	//************************************
	// Method:      DataFull
	// FullName:    CAudio::DataFull
	// Access:      private 
	// Returns:     bool
	// Qualifier:   const
	// Description: 
	//************************************
	inline bool DataFull() const { return m_dwStart < m_dwEnd ? m_dwStart + 1 == m_dwEnd : 0 == m_dwEnd&&MAX_WAVE_DATA_COUNT - 1 == m_dwStart; }
	
	//************************************
	// Method:      DataEmpty
	// FullName:    CAudio::DataEmpty
	// Access:      private 
	// Returns:     bool
	// Qualifier:   const
	// Description: 
	//************************************
	inline bool DataEmpty() const { return m_dwStart == m_dwCur; }
private:
	HWAVEOUT m_hWaveOut;
	static DATA m_cData[MAX_WAVE_DATA_COUNT];	// 循环队列，用于缓存音频数据
	DWORD m_dwStart;		// data循环队列的队首
	DWORD m_dwCur;			// 开始播放data循环队列中数据的索引
	DWORD m_dwEnd;			// data循环队列的队尾
	DWORD m_dwWaveHdrIndex;	// 当前使用的waveHdr的索引
	WAVEHDR m_cWaveHdr[MAX_WAVE_COUNT];	// 用于waveOutWrite的缓冲区,实现多缓冲播放
	DWORD m_dwCount;		// 当前正在播放的wave的数量
	HANDLE m_hDataEvent,	// 一个wave播放完事件
		m_hNotFullEvent, m_hNotEmptyEvent;	// data循环队列相关事件
	HANDLE m_hThread;		// 从data循环队列播放音频的线程
	BOOL m_bContinue,		// 播放线程是否继续
		m_bForce;			// TRUE:按ESC退出,FALSE:播放完退出
	bool &m_bAudioStart, &m_bVideoStart;	// 音/视频是否已开始播放
	HANDLE m_hStartEvent;	// 开始播放事件
};

