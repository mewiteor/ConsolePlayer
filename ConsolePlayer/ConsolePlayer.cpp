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
#include<iostream>
#include<memory>
#include<string>
#include<conio.h>
#include<windows.h>
#include "common.h"
#include "FileOpenDialog.h"
#include "Exception.h"
#include "Console.h"
#include "FFMpeg.h"

using namespace std;

//************************************
// Method:      Play
// FullName:    Play
// Access:      public 
// Returns:     void
// Qualifier:  
// Parameter:   const char * path
// Parameter:   ColorType colorType
// Description: 
//************************************
void Play(const char* path, ColorType colorType);

// 默认调色板
COLORREF colorTable[16] =
{
	0x00000000,0x00800000,0x00008000,0x00808000,
	0x00000080,0x00800080,0x00008080,0x00c0c0c0,
	0x00808080,0x00ff0000,0x0000ff00,0x00ffff00,
	0x000000ff,0x00ff00ff,0x0000ffff,0x00ffffff
};

int main()
{
	try
	{
		CFileOpenDialog ofd;
		if (ofd.Show())
		{
			char file_name[MAX_PATH];
			ColorType colorType;
			ofd.GetResult(file_name, colorType);
			Play(file_name, colorType);
		}
	}
	catch (const CException &err)
	{
		cerr << err.what() << endl;
		system("pause");
	}
	catch (const char* err)
	{
		cerr << err << endl;
		system("pause");
	}
	return 0;
}

void Play(const char* path, ColorType colorType)
{
	CConsole::SetTitle("准备播放:%s", path);
	CFFMpeg cff(path);
	cff.InitVideo();
	cff.InitAudio();
	int inWidth = 0, inHeight = 0;
	cff.GetInSize(inWidth, inHeight);
	if (!inWidth || !inHeight)return;
	COORD cr;
	CConsole c;
	if (!c.GetSize().X || !c.GetSize().Y)return;
	if (inWidth + CConsole::m_DEFAULT_FONT_COORD.X - 1 <= c.GetSize().X*CConsole::m_DEFAULT_FONT_COORD.X&&
		inHeight + CConsole::m_DEFAULT_FONT_COORD.Y - 1 <= c.GetSize().Y*CConsole::m_DEFAULT_FONT_COORD.Y)
	{
		cr.X = (inWidth + CConsole::m_DEFAULT_FONT_COORD.X - 1) / CConsole::m_DEFAULT_FONT_COORD.X;
		cr.Y = (inHeight + CConsole::m_DEFAULT_FONT_COORD.Y - 1) / CConsole::m_DEFAULT_FONT_COORD.Y;
	}
	else
	{
		double bd = double(inWidth) / inHeight,
			cd = double(c.GetSize().X*CConsole::m_DEFAULT_FONT_COORD.X) / c.GetSize().Y / CConsole::m_DEFAULT_FONT_COORD.Y;
		if (bd <= cd)
		{
			cr.Y = c.GetSize().Y;
			cr.X = SHORT(c.GetSize().Y*CConsole::m_DEFAULT_FONT_COORD.Y*bd / CConsole::m_DEFAULT_FONT_COORD.X);
		}
		else
		{
			cr.X = c.GetSize().X;
			cr.Y = SHORT(c.GetSize().X*CConsole::m_DEFAULT_FONT_COORD.X / bd / CConsole::m_DEFAULT_FONT_COORD.Y);
		}
	}
	c.SetSize(cr);
	cff.SetOutSize(cr.X, cr.Y);
	if (ColorType::MONO == colorType)
		for (int i = 0, j = 0; i < 0x1000000; i += 0x111111, ++j)
			colorTable[j] = i;
	c.SetColorTable(colorTable);
	bool bWaveStart = false, bVideoStart = false;
	auto closeHandle = [](HANDLE p) {CloseHandle(p); };
	unique_ptr<void, decltype(closeHandle)> hStartEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr), closeHandle);
	HANDLE hStartEvent_ = hStartEvent.get();
	CAudio audio((WORD)cff.GetChannels(), (DWORD)cff.GetSampleRate(), (WORD)cff.GetBitsPerSample(), bWaveStart, bVideoStart, hStartEvent_);
	CVideo video(c, (DWORD)cff.GetDuration(), colorType, bWaveStart, bVideoStart, hStartEvent.get(), path, colorTable);
	bool bForce = cff.ReadFrames([](CFFMpeg::StreamType type, uint8_t * data, int len, int64_t ts, CAudio& audio, CVideo& video)
	{
		if (_kbhit() && 0x1b == _getch())return true;
		switch (type)
		{
		case CFFMpeg::StreamType::AUDIO:
			audio.Push(data, len);
			break;
		case CFFMpeg::StreamType::VIDEO:
			video.Push(data, len, ts);
			break;
		}
		return false;
	}, audio, video);
	audio.Stop(bForce);
	video.Stop(bForce);
}

