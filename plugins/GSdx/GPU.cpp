/* 
 *	Copyright (C) 2007-2009 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"
#include "GSdx.h"
#include "GSUtil.h"
#include "GPURendererSW.h"
#include "GSDevice7.h"
#include "GSDevice9.h"
#include "GSDevice10.h"
#include "GPUSettingsDlg.h"

#define PSE_LT_GPU 2

static HRESULT s_hr = E_FAIL;
static GPURendererBase* s_gpu = NULL;

EXPORT_C_(uint32) PSEgetLibType()
{
	return PSE_LT_GPU;
}

EXPORT_C_(char*) PSEgetLibName()
{
	return GSUtil::GetLibName();
}

EXPORT_C_(uint32) PSEgetLibVersion()
{
	static const uint32 version = 1;
	static const uint32 revision = 1;

	return version << 16 | revision << 8 | PLUGIN_VERSION;
}

EXPORT_C_(int32) GPUinit()
{
	return 0;
}

EXPORT_C_(int32) GPUshutdown()
{
	return 0;
}

EXPORT_C_(int32) GPUclose()
{
	delete s_gpu; 
	
	s_gpu = NULL;

#ifdef _WINDOWS

	if(SUCCEEDED(s_hr))
	{
		::CoUninitialize();

		s_hr = E_FAIL;
	}

#endif

	return 0;
}

EXPORT_C_(int32) GPUopen(HWND hWnd)
{
	GPUclose();

	GPURendererSettings rs;

	int threads = 1;
	int renderer = 1;

#ifdef _WINDOWS

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	s_hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if(!GSUtil::CheckDirectX())
	{
		return -1;
	}

#endif

	if(!GSUtil::CheckSSE())
	{
		return -1;
	}

	rs.m_filter = theApp.GetConfig("filter", 0);
	rs.m_dither = theApp.GetConfig("dithering", 1);
	rs.m_aspectratio = theApp.GetConfig("AspectRatio", 1);
	rs.m_vsync = !!theApp.GetConfig("vsync", 0);
	rs.m_scale.x = theApp.GetConfig("scale_x", 0);
	rs.m_scale.y = theApp.GetConfig("scale_y", 0);

	threads = theApp.GetConfig("swthreads", 1);
	renderer = theApp.GetConfig("Renderer", 1);

	switch(renderer)
	{
	default: 
	case 0: s_gpu = new GPURendererSW<GSDevice7>(rs, threads); break;
	case 1: s_gpu = new GPURendererSW<GSDevice9>(rs, threads); break;
	case 2: s_gpu = new GPURendererSW<GSDevice10>(rs, threads); break;
	// TODO: case 3: s_gpu = new GPURendererNull<GSDeviceNull>(rs, threads); break;
	}

	if(!s_gpu->Create(hWnd))
	{
		GPUclose();

		return -1;
	}

	return 0;
}

EXPORT_C_(int32) GPUconfigure()
{

#ifdef _WINDOWS
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GPUSettingsDlg dlg;

	if(IDOK == dlg.DoModal())
	{
		GPUshutdown();
		GPUinit();
	}

#endif

	return 0;
}

EXPORT_C_(int32) GPUtest()
{
	return 0;
}

EXPORT_C GPUabout()
{
	// TODO
}

EXPORT_C GPUwriteDataMem(const uint8* mem, uint32 size)
{
	s_gpu->WriteData(mem, size);
}

EXPORT_C GPUwriteData(uint32 data)
{
	s_gpu->WriteData((uint8*)&data, 1);
}

EXPORT_C GPUreadDataMem(uint8* mem, uint32 size)
{
	s_gpu->ReadData(mem, size);
}

EXPORT_C_(uint32) GPUreadData()
{
	uint32 data = 0;

	s_gpu->ReadData((uint8*)&data, 1);

	return data;
}

EXPORT_C GPUwriteStatus(uint32 status)
{
	s_gpu->WriteStatus(status);
}

EXPORT_C_(uint32) GPUreadStatus()
{
	return s_gpu->ReadStatus();
}

EXPORT_C_(uint32) GPUdmaChain(const uint8* mem, uint32 addr)
{
	uint32 last[3];

	memset(last, 0xff, sizeof(last));

	do
	{
		if(addr == last[1] || addr == last[2]) 
		{
			break;
		}

		(addr < last[0] ? last[1] : last[2]) = addr;
		
		last[0] = addr;

		uint8 size = mem[addr + 3];

		if(size > 0)
		{
			s_gpu->WriteData(&mem[addr + 4], size);
		}

		addr = *(uint32*)&mem[addr] & 0xffffff;
	}
	while(addr != 0xffffff);

	return 0;
}

EXPORT_C_(uint32) GPUgetMode()
{
	// TODO

	return 0;
}

EXPORT_C GPUsetMode(uint32 mode)
{
	// TODO
}

EXPORT_C GPUupdateLace()
{
	s_gpu->VSync();
}

EXPORT_C GPUmakeSnapshot()
{
	s_gpu->MakeSnapshot("c:/"); // TODO
}

EXPORT_C GPUdisplayText(char* text)
{
	// TODO
}

EXPORT_C GPUdisplayFlags(uint32 flags)
{
	// TODO
}

EXPORT_C_(int32) GPUfreeze(uint32 type, GPUFreezeData* data)
{
	if(!data || data->version != 1)
	{
		return 0;
	}

	if(type == 0)
	{
		s_gpu->Defrost(data);

		return 1;
	}
	else if(type == 1)
	{
		s_gpu->Freeze(data);

		return 1;
	}
	else if(type == 2)
	{
		int slot = *(int*)data + 1;
		
		if(slot < 1 || slot > 9)
		{
			return 0;
		}

		// TODO
		
		return 1;
	}

	return 0;
}

EXPORT_C GPUgetScreenPic(uint8* mem)
{
	// TODO
}

EXPORT_C GPUshowScreenPic(uint8* mem)
{
	// TODO
}

EXPORT_C GPUcursor(int player, int x, int y)
{
	// TODO
}