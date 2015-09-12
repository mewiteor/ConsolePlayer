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
#include <wtypes.h>
#include<cstdint>
#include "common.h"
#include "Console.h"

class CColorQuantization;
class CErrorDiffusion;
class CBayer;

 /*!
  * \class CVideo
  *
  * \brief 视频播放类
  *
  * \author Mewiteor
  * \date 九月 2015
  */
class CVideo
{
	enum { MAX_VIDEO_DATA_COUNT = 64 };
	struct ATTR
	{
		WORD* attr;					// 控制台属性
		COLORREF colorTable[16];	// 控制台调色板
		int64_t timestamp;			// 时间戳
	};
	struct DATA
	{
		uint8_t * data;				// 视频数据,RGB序列,长度为控制台屏幕流总长度的3倍
		int64_t timestamp;			// 时间戳
	};
public:

	//************************************
	// Method:      CVideo
	// FullName:    CVideo::CVideo
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   const CConsole & console
	// Parameter:   DWORD duration
	// Parameter:   ColorType colorType
	// Parameter:   bool & audioStart
	// Parameter:   bool & videoStart
	// Parameter:   HANDLE hStartEvent
	// Parameter:   const char * filePath
	// Parameter:   COLORREF defaultColorTable[16]
	// Description: 
	//************************************
	CVideo(const CConsole &console, DWORD duration, ColorType colorType,
		bool &audioStart, bool &videoStart, HANDLE hStartEvent,
		const char* filePath, COLORREF defaultColorTable[16] = nullptr);

	//************************************
	// Method:      ~CVideo
	// FullName:    CVideo::~CVideo
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Description: 
	//************************************
	~CVideo();

	//************************************
	// Method:      Push
	// FullName:    CVideo::Push
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   unsigned char * buf
	// Parameter:   size_t len
	// Parameter:   int64_t timestamp
	// Description: 压入视频数据到data循环队列
	//************************************
	void Push(unsigned char *buf, size_t len, int64_t timestamp);

	//************************************
	// Method:      Stop
	// FullName:    CVideo::Stop
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   bool force,true:按ESC退出,false:播放完退出
	// Description: 播放结束
	//************************************
	void Stop(bool force);

private:
	//************************************
	// Method:      ColorQuantizationThread
	// FullName:    CVideo::ColorQuantizationThread
	// Access:      private static 
	// Returns:     DWORD CALLBACK
	// Qualifier:  
	// Parameter:   LPVOID lp
	// Description: 
	//************************************
	static DWORD CALLBACK ColorQuantizationThread(LPVOID lp);
	
	//************************************
	// Method:      VideoColorQuantizationThread
	// FullName:    CVideo::VideoColorQuantizationThread
	// Access:      private 
	// Returns:     DWORD
	// Qualifier:  
	// Description: 
	//************************************
	DWORD VideoColorQuantizationThread();
	
	//************************************
	// Method:      WriteAttrThread
	// FullName:    CVideo::WriteAttrThread
	// Access:      private static 
	// Returns:     DWORD CALLBACK
	// Qualifier:  
	// Parameter:   LPVOID lp
	// Description: 
	//************************************
	static DWORD CALLBACK WriteAttrThread(LPVOID lp);
	
	//************************************
	// Method:      VideoWriteAttrThread
	// FullName:    CVideo::VideoWriteAttrThread
	// Access:      private 
	// Returns:     DWORD
	// Qualifier:  
	// Description: 
	//************************************
	DWORD VideoWriteAttrThread();

	//************************************
	// Method:      DataFull
	// FullName:    CVideo::DataFull
	// Access:      private 
	// Returns:     bool
	// Qualifier:   const
	// Description: 
	//************************************
	inline bool DataFull()const
	{
		return m_nDataStart < m_nDataToAttrIndex ?
			m_nDataStart + 1 == m_nDataToAttrIndex :
			!m_nDataToAttrIndex&&m_nDataStart == MAX_VIDEO_DATA_COUNT - 1;
	}
	
	//************************************
	// Method:      DataEmpty
	// FullName:    CVideo::DataEmpty
	// Access:      private 
	// Returns:     bool
	// Qualifier:   const
	// Description: 
	//************************************
	inline bool DataEmpty()const { return m_nDataStart == m_nDataToAttrIndex; }
	
	//************************************
	// Method:      AttrFull
	// FullName:    CVideo::AttrFull
	// Access:      private 
	// Returns:     bool
	// Qualifier:   const
	// Description: 
	//************************************
	inline bool AttrFull()const
	{
		return m_nDataToAttrIndex < m_nAttrEnd ?
			m_nDataToAttrIndex + 1 == m_nAttrEnd :
			!m_nAttrEnd&&m_nDataToAttrIndex == MAX_VIDEO_DATA_COUNT - 1;
	}
	
	//************************************
	// Method:      AttrEmpty
	// FullName:    CVideo::AttrEmpty
	// Access:      private 
	// Returns:     bool
	// Qualifier:   const
	// Description: 
	//************************************
	inline bool AttrEmpty()const { return m_nDataToAttrIndex == m_nAttrEnd; }
	
	//************************************
	// Method:      Convert24bppTo4bpp
	// FullName:    CVideo::Convert24bppTo4bpp
	// Access:      private static 
	// Returns:     WORD
	// Qualifier:  
	// Parameter:   short red
	// Parameter:   short green
	// Parameter:   short blue
	// Description: 将24位颜色转化为最接近的控制台默认调色板上的4位颜色
	//************************************
	static WORD Convert24bppTo4bpp(short red, short green, short blue);

private:
	const CConsole &m_cConsole;
	ColorType m_cColorType;
	ATTR m_cAttributes[MAX_VIDEO_DATA_COUNT];		// 控制台属性循环队列
	DWORD m_dwStartTime,		// 开始播放的时间,单位ms
		m_dwLastTime,			// 上一帧的时间,单位ms
		m_dwLastTimeSeconds;	// 上一帧的时间,单位s
	DWORD m_dwDuration;			// 视频总时长,单位s
	unsigned int m_nFrames;	// 记录帧数,用于计算fps
	float m_fLastFps;			// 上一次计算过的fps
	CColorQuantization* m_pColorQuantization;
	CErrorDiffusion* m_pErrorDiffusion;
	CBayer* m_pBayer;
	size_t m_nLength;			// 控制台屏幕流的总长度(console.GetSize().X*console.GetSize().Y)
	bool m_bContinue,			// 播放线程是否继续
		m_bForce;				// TRUE:按ESC退出,FALSE:播放完退出
	HANDLE m_hColorQuantizationThread, m_hWriteAttrThread;
	DATA m_cData[MAX_VIDEO_DATA_COUNT];	// 帧RGB数据循环队列
	size_t m_nDataStart,		// 帧RGB数据循环队列的队首
		m_nDataToAttrIndex,	// 帧RGB数据循环队列的队尾,控制台属性循环队列的队首,这两个索引一直是相等的
		m_nAttrEnd;			// 控制台属性循环队列的队尾
	HANDLE m_hDataNotEmptyEvent,
		m_hAttrNotFullEvent,
		m_hConvertDataToAttrEvent;
	bool &m_bAudioStart, &m_bVideoStart;
	HANDLE m_hStartEvent;		// 开始播放事件
	const char* m_strPlayFilePath;
	const COLORREF* m_pDefaultColorTable;	// 默认调色板,用于ColorType::MONO(黑白)、ColorType::STANDARD(标准)两种颜色模式
	DWORD m_dwPeriodMin;	// 计时精度的最小值
};

