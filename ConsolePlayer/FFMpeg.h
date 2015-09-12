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
extern "C"
{
#include<libavcodec\avcodec.h>
#include<libavformat\avformat.h>
#include<libswscale\swscale.h>
#include<libswresample\swresample.h>
}
#include "Audio.h"
#include "Video.h"

/*!
 * \class CFFMpeg
 *
 * \brief 对ffmpeg解码器的封装
 *
 * \author Mewiteor
 * \date 九月 2015
 */
class CFFMpeg
{
public:
	enum StreamType
	{
		AUDIO,
		VIDEO
	};
public:

	//************************************
	// Method:      CFFMpeg
	// FullName:    CFFMpeg::CFFMpeg
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Parameter:   const char * path
	// Description: 
	//************************************
	CFFMpeg(const char* path);
	
	//************************************
	// Method:      ~CFFMpeg
	// FullName:    CFFMpeg::~CFFMpeg
	// Access:      public 
	// Returns:     
	// Qualifier:  
	// Description: 
	//************************************
	~CFFMpeg();
	
	//************************************
	// Method:      InitVideo
	// FullName:    CFFMpeg::InitVideo
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Description: 
	//************************************
	void InitVideo();
	
	//************************************
	// Method:      InitAudio
	// FullName:    CFFMpeg::InitAudio
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Description: 
	//************************************
	void InitAudio();
	
	//************************************
	// Method:      SetOutSize
	// FullName:    CFFMpeg::SetOutSize
	// Access:      public 
	// Returns:     void
	// Qualifier:  
	// Parameter:   int width
	// Parameter:   int height
	// Description: 设置输出分辨率
	//************************************
	void SetOutSize(int width, int height);
	
	//************************************
	// Method:      GetInSize
	// FullName:    CFFMpeg::GetInSize
	// Access:      public 
	// Returns:     void
	// Qualifier:   const
	// Parameter:   int & width
	// Parameter:   int & height
	// Description: 返回输入分辨率
	//************************************
	void GetInSize(int &width, int &height)const;
	
	//************************************
	// Method:      ReadFrames
	// FullName:    CFFMpeg::ReadFrames
	// Access:      public 
	// Returns:     bool
	// Qualifier:  
	// Parameter:   bool(*fnFrameProc)(StreamType, uint8_t*, int, int64_t, CAudio&, CVideo&)
	// Parameter:   CAudio & audio
	// Parameter:   CVideo & video
	// Description: 
	//************************************
	bool ReadFrames(bool(*fnFrameProc)(StreamType, uint8_t*, int, int64_t, CAudio&, CVideo&), CAudio& audio, CVideo& video);
	
	//************************************
	// Method:      GetChannels
	// FullName:    CFFMpeg::GetChannels
	// Access:      public 
	// Returns:     int
	// Qualifier:   const
	// Description: 
	//************************************
	inline int  GetChannels()const { return m_nChannels; }
	
	//************************************
	// Method:      GetSampleRate
	// FullName:    CFFMpeg::GetSampleRate
	// Access:      public 
	// Returns:     int
	// Qualifier:   const
	// Description: 
	//************************************
	inline int  GetSampleRate()const { return m_nSampleRate; }
	
	//************************************
	// Method:      GetBitsPerSample
	// FullName:    CFFMpeg::GetBitsPerSample
	// Access:      public 
	// Returns:     int
	// Qualifier:   const
	// Description: 
	//************************************
	inline int  GetBitsPerSample()const { return m_nBitsPerSample; }
	
	//************************************
	// Method:      GetDuration
	// FullName:    CFFMpeg::GetDuration
	// Access:      public 
	// Returns:     uint64_t
	// Qualifier:   const
	// Description: 
	//************************************
	inline uint64_t GetDuration()const { return m_pFormatCtx->duration / AV_TIME_BASE; }

private:
	AVFormatContext* m_pFormatCtx;
	const char* m_strPath;
	AVCodecContext* m_pVideoCodecCtx, *m_pAudioCodecCtx;
	int m_nInWidth, m_nInHeight, m_nOutWidth, m_nOutHeight;
	int m_nVideoIndex, m_nAudioIndex;
	const int m_nSampleRate, m_nBitsPerSample;
	int m_nChannels;
	const AVSampleFormat m_avSampleFormat;
	uint64_t m_nVideoFrameCount;
};

