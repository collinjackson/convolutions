// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifdef WIN32

#include "CIrrDeviceWin32.h"
#include "IEventReceiver.h"
#include "irrlist.h"
#include "os.h"
#include "IrrCompileConfig.h"
#include "CTimer.h"
#include <stdio.h>
#include "irrstring.h"
#include "COSOperator.h"
#include "dimension2d.h"
#include <winuser.h>

namespace irr
{
	namespace video
	{
		IVideoDriver* createDirectX8Driver(const core::dimension2d<s32>& screenSize, HWND window, u32 bits, bool fullscreen, bool stencilbuffer, io::IFileSystem* io, bool pureSoftware);
		IVideoDriver* createDirectX9Driver(const core::dimension2d<s32>& screenSize, HWND window, u32 bits, bool fullscreen, bool stencilbuffer, io::IFileSystem* io, bool pureSoftware);
		IVideoDriver* createOpenGLDriver(const core::dimension2d<s32>& screenSize, HWND window, bool fullscreen, bool stencilBuffer, io::IFileSystem* io);
	}

} // end namespace irr



struct SEnvMapper
{
	HWND hWnd;
	irr::CIrrDeviceWin32* irrDev;
};

irr::core::list<SEnvMapper> EnvMap;

SEnvMapper* getEnvMapperFromHWnd(HWND hWnd)
{
	irr::core::list<SEnvMapper>::Iterator it = EnvMap.begin();
	for (; it!= EnvMap.end(); ++it)
		if ((*it).hWnd == hWnd)
			return &(*it);

	return 0;
}

irr::CIrrDeviceWin32* getDeviceFromHWnd(HWND hWnd)
{
	irr::core::list<SEnvMapper>::Iterator it = EnvMap.begin();
	for (; it!= EnvMap.end(); ++it)
		if ((*it).hWnd == hWnd)
			return (*it).irrDev;

	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
	#endif
	#ifndef WHEEL_DELTA                     
	#define WHEEL_DELTA 120
	#endif

	irr::CIrrDeviceWin32* dev = 0;
	irr::SEvent event;
	SEnvMapper* envm = 0;

	switch (message) 
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_SETCURSOR: 
		envm = getEnvMapperFromHWnd(hWnd);
		if (envm &&	!envm->irrDev->getWin32CursorControl()->isVisible())
		{
			SetCursor(NULL); 
			return 0; 
		}
        break;

	case WM_MOUSEWHEEL: 
		event.EventType = irr::EET_MOUSE_INPUT_EVENT; 
		event.MouseInput.Wheel = (float)((short)HIWORD(wParam)) / (float)WHEEL_DELTA;
		event.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
		event.MouseInput.X = LOWORD(lParam); 
		event.MouseInput.Y = HIWORD(lParam); 

		dev = getDeviceFromHWnd(hWnd); 
		if (dev) 
			dev->postEventFromUser(event); 
		break;

	case WM_LBUTTONDOWN:
        event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_LBUTTONUP:
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_RBUTTONDOWN:
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_RBUTTONUP:
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_MBUTTONDOWN:
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_MBUTTONUP:
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_MOUSEMOVE:
		event.EventType = irr::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->postEventFromUser(event);
		return 0;

	case WM_KEYDOWN:
		{
			event.EventType = irr::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = true;
			dev = getDeviceFromHWnd(hWnd);

			BYTE allKeys[256];
			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii(wParam,lParam,allKeys,&KeyAsc,0);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			if (dev)
				dev->postEventFromUser(event);

			return 0;
		}
	case WM_KEYUP:
		{
			event.EventType = irr::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (irr::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = false;
			dev = getDeviceFromHWnd(hWnd);

			BYTE allKeys[256];
			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii(wParam,lParam,allKeys,&KeyAsc,0);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			if (dev)
				dev->postEventFromUser(event);

			return 0;
		}

	case WM_SIZE:
		{
			// resize
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->OnResized();
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if (wParam == SC_SCREENSAVE ||
			wParam == SC_MONITORPOWER)
			return 0;
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

namespace irr
{

//! constructor
CIrrDeviceWin32::CIrrDeviceWin32(video::EDriverType driverType, 
								 const core::dimension2d<s32>& windowSize,
								 u32 bits, bool fullscreen, 
								 bool stencilbuffer, IEventReceiver* receiver,
								 const wchar_t* version)
: CIrrDeviceStub(version, receiver), HWnd(0), ChangedToFullScreen(false),
	Win32CursorControl(0), IsNonNTWindows(false), Resized(false), FullScreen(fullscreen)
{
	core::stringc winversion;
	getWindowsVersion(winversion);
	Operator = new COSOperator(winversion.c_str());
	os::Printer::log(winversion.c_str(), ELL_INFORMATION);

	// create window

	HINSTANCE hInstance = GetModuleHandle(0);

	#ifdef _DEBUG
	setDebugName("CIrrDeviceWin32");	
	#endif

	if (driverType != video::EDT_NULL)
	{
		// create the window, only if we do not use the null device

		const c8* ClassName = "CIrrDeviceWin32";

		// Register Class

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX); 
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= ClassName;
		wcex.hIconSm		= 0;

		RegisterClassEx(&wcex);

		// calculate client size

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = windowSize.Width;
		clientSize.bottom = windowSize.Height;
		
		DWORD style = WS_POPUP;

		if (!fullscreen)
			style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		AdjustWindowRect(&clientSize, style, FALSE);

		s32 realWidth = clientSize.right - clientSize.left;
		s32 realHeight = clientSize.bottom - clientSize.top;

		s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if (fullscreen)
		{
			windowLeft = 0;
			windowTop = 0;
		}

		// create window

		HWnd = CreateWindow( ClassName, "", style, windowLeft, windowTop, 
					realWidth, realHeight,	NULL, NULL, hInstance, NULL);

		ShowWindow(HWnd , SW_SHOW);
		UpdateWindow(HWnd);

		// fix ugly ATI driver bugs. Thanks to ariaci
		MoveWindow(HWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);
	}

	// create cursor control

	Win32CursorControl = new CCursorControl(windowSize, HWnd, fullscreen);
	CursorControl = Win32CursorControl;

	// create driver

	createDriver(driverType, windowSize, bits, fullscreen, stencilbuffer);

	createGUIAndScene();

	// register environment

	SEnvMapper em;
	em.irrDev = this;
	em.hWnd = HWnd;
	EnvMap.push_back(em);

}



//! destructor
CIrrDeviceWin32::~CIrrDeviceWin32()
{
	// unregister environment

	irr::core::list<SEnvMapper>::Iterator it = EnvMap.begin();
	for (; it!= EnvMap.end(); ++it)
		if ((*it).hWnd == HWnd)
		{
			EnvMap.erase(it);
			break;
		}

	if (ChangedToFullScreen)
		ChangeDisplaySettings(NULL,0);
}



//! create the driver
void CIrrDeviceWin32::createDriver(video::EDriverType driverType,
								   const core::dimension2d<s32>& windowSize,
								   u32 bits, 
								   bool fullscreen,
								   bool stencilbuffer)
{
	switch(driverType)
	{
	case video::EDT_DIRECTX8:
		#ifdef _IRR_COMPILE_WITH_DIRECTX_8_
		VideoDriver = video::createDirectX8Driver(windowSize, HWnd, bits, fullscreen, 
			stencilbuffer, FileSystem, false);
		#endif

		if (!VideoDriver)
		{
			#ifdef _IRR_COMPILE_WITH_DIRECTX_8_
			os::Printer::log("Could not create DirectX8 Driver.", ELL_ERROR);
			#else
			os::Printer::log("DirectX Driver was not compiled into this dll. Try OpenGL.", ELL_ERROR);
			#endif
		}

		break;	

	case video::EDT_DIRECTX9:
		#ifdef _IRR_COMPILE_WITH_DIRECTX_9_
		VideoDriver = video::createDirectX9Driver(windowSize, HWnd, bits, fullscreen, 
			stencilbuffer, FileSystem, false);
		#endif

		if (!VideoDriver)
		{
			#ifdef _IRR_COMPILE_WITH_DIRECTX_9_
			os::Printer::log("Could not create DirectX9 Driver.", ELL_ERROR);
			#else
			os::Printer::log("DirectX Driver was not compiled into this dll. Try OpenGL.", ELL_ERROR);
			#endif
		}

		break;	
	
	case video::EDT_OPENGL:

		#ifdef _IRR_COMPILE_WITH_OPENGL_
		if (fullscreen)	switchToFullScreen(windowSize.Width, windowSize.Height, bits);
		VideoDriver = video::createOpenGLDriver(windowSize, HWnd, fullscreen, stencilbuffer, FileSystem);
		#endif
		
		if (!VideoDriver)
		{
			#ifdef _IRR_COMPILE_WITH_OPENGL_
			os::Printer::log("Could not create OpenGL driver.", ELL_ERROR);
			#else
			os::Printer::log("OpenGL driver was not compiled into this dll.", ELL_ERROR);
			#endif
		}
		break;	

	case video::EDT_SOFTWARE:
		if (fullscreen)	switchToFullScreen(windowSize.Width, windowSize.Height, bits);
		VideoDriver = video::createSoftwareDriver(windowSize, fullscreen, FileSystem, this);
		break;

	default:
		// create null driver if a bad programmer made a mistake
		VideoDriver = video::createNullDriver(FileSystem, windowSize);
		break;
	}
}



//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceWin32::run()
{
	MSG msg;

	/*
	IsNonNTWindows = true;

	if (!IsNonNTWindows)
	{
		// Define PM_QS_INPUT for DevC++ and VS6, its not defined in their headers. 

		#ifndef PM_QS_INPUT 
		#define PM_QS_INPUT (QS_INPUT << 16)
		#endif	

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_QS_INPUT ))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		msg.message = WM_NULL;
	}
	*/

	bool quit = false;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			quit = true;
	}

	if (!quit)
		resizeIfNecessary();

	return !quit;
}


void CIrrDeviceWin32::resizeIfNecessary()
{
	if (!Resized)
		return;

	RECT r;
	GetClientRect(HWnd, &r);

	char tmp[255];

	if (r.right <= 1 || r.bottom <= 1)
	{
		sprintf(tmp, "Ignoring resize opetation to (%d %d)", r.right, r.bottom);
		os::Printer::log(tmp);
	}
	else
	{
		sprintf(tmp, "Resizing window (%d %d)", r.right, r.bottom);
		os::Printer::log(tmp);

		getVideoDriver()->OnResize(irr::core::dimension2d<irr::s32>(r.right, r.bottom));
	}

	Resized = false;
}



//! sets the caption of the window
void CIrrDeviceWin32::setWindowCaption(const wchar_t* text)
{
	if (IsNonNTWindows)
	{
		core::stringc s = text;
		SetWindowTextA(HWnd, s.c_str());
	}
	else
		SetWindowTextW(HWnd, text);
}



//! presents a surface in the client area
void CIrrDeviceWin32::present(video::IImage* image)
{
	HDC dc = GetDC(HWnd);
	RECT rect;
	GetClientRect(HWnd, &rect);

	if (image->getColorFormat() != video::ECF_A1R5G5B5)
		return;

	s16* memory = (s16*)image->lock();
			
	BITMAPV4HEADER bi;
	ZeroMemory (&bi, sizeof(bi));
	bi.bV4Size          = sizeof(BITMAPINFOHEADER);
	bi.bV4BitCount      = 16;
	bi.bV4Planes        = 1;
	bi.bV4Width         = image->getDimension().Width;
	bi.bV4Height        = -image->getDimension().Height;
	bi.bV4V4Compression = BI_BITFIELDS;
	bi.bV4AlphaMask     = (s16)(0x1<<15);
	bi.bV4RedMask       = 0x1F<<10;
	bi.bV4GreenMask     = 0x1F<<5;
	bi.bV4BlueMask      = 0x1F;

	StretchDIBits(dc, 0,0, rect.right, rect.bottom,
						0, 0, image->getDimension().Width, image->getDimension().Height,
						memory, (const BITMAPINFO*)(&bi), DIB_RGB_COLORS, SRCCOPY);

	image->unlock();

	ReleaseDC(HWnd, dc);
}



//! notifies the device that it should close itself
void CIrrDeviceWin32::closeDevice()
{
	DestroyWindow(HWnd); 	//PostQuitMessage(0);
}



//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceWin32::isWindowActive()
{
	return (GetActiveWindow() == HWnd);
}



//! switchs to fullscreen
bool CIrrDeviceWin32::switchToFullScreen(s32 width, s32 height, s32 bits)
{
	DEVMODE dm;
	memset(&dm, 0, sizeof(dm));
	dm.dmSize = sizeof(dm);
	dm.dmPelsWidth = width;
	dm.dmPelsHeight	= height;
	dm.dmBitsPerPel	= bits;
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	LONG ret = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	
	switch(ret)
	{
	case DISP_CHANGE_SUCCESSFUL:
		ChangedToFullScreen = true;
		return true;
	case DISP_CHANGE_RESTART:
		os::Printer::log("Switch to fullscreen: The computer must be restarted in order for the graphics mode to work.", ELL_ERROR);
		return false;
	case DISP_CHANGE_BADFLAGS:
		os::Printer::log("Switch to fullscreen: An invalid set of flags was passed in.", ELL_ERROR);
		return false;
	case DISP_CHANGE_BADPARAM:
		os::Printer::log("Switch to fullscreen: An invalid parameter was passed in. This can include an invalid flag or combination of flags.", ELL_ERROR);
		return false;
	case DISP_CHANGE_FAILED:
		os::Printer::log("Switch to fullscreen: The display driver failed the specified graphics mode.", ELL_ERROR);
		return false;
	case DISP_CHANGE_BADMODE:
		os::Printer::log("Switch to fullscreen: The graphics mode is not supported.", ELL_ERROR);
		return false;
	}

	os::Printer::log("An unknown error occured while changing to fullscreen.", ELL_ERROR);
	return false;
}


//! returns the win32 cursor control
CIrrDeviceWin32::CCursorControl* CIrrDeviceWin32::getWin32CursorControl()
{
	return Win32CursorControl;
}


//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
video::IVideoModeList* CIrrDeviceWin32::getVideoModeList()
{
	if (!VideoModeList.getVideoModeCount())
	{
		// enumerate video modes.
		DWORD i=0;
		DEVMODE mode;

		while (EnumDisplaySettings(NULL, i, &mode))
		{
			VideoModeList.addMode(core::dimension2d<s32>(mode.dmPelsWidth, mode.dmPelsHeight),
				mode.dmBitsPerPel);

			++i;
		}

		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode))
			VideoModeList.setDesktop(mode.dmBitsPerPel, core::dimension2d<s32>(mode.dmPelsWidth, mode.dmPelsHeight));
	}

	return &VideoModeList;
}


void CIrrDeviceWin32::getWindowsVersion(core::stringc& out)
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize	= sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO*) &osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 4)
			out.append("Microsoft Windows NT ");
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			out.append("Microsoft Windows 2000 ");
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			out.append("Microsoft Windows XP ");

		if( bOsVersionInfoEx )
		{
			#ifdef VER_SUITE_ENTERPRISE
						if (osvi.wProductType == VER_NT_WORKSTATION)
						{
							if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
								out.append("Personal ");
							else
								out.append("Professional ");
						}
						else if (osvi.wProductType == VER_NT_SERVER)
						{
							if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
								out.append("DataCenter Server ");
						   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							   out.append("Advanced Server ");
						   else
							   out.append("Server ");
						}
			#endif
		}
		else
		{
			HKEY hKey;
			char szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 
				0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey );

			if (lstrcmpi( "WINNT", szProductType) == 0 )
				out.append("Professional ");
			if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
				out.append("Server " );
			if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
				out.append("Advanced Server ");
         }

		// Display version, service pack (if any), and build number.
		
		char tmp[255];

		if (osvi.dwMajorVersion <= 4 )
		{
			sprintf (tmp, "version %d.%d %s (Build %d)",
				osvi.dwMajorVersion,
				osvi.dwMinorVersion,
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}
		else
		{
			sprintf (tmp, "%s (Build %d)", osvi.szCSDVersion, 
				osvi.dwBuildNumber & 0xFFFF);
		}

		out.append(tmp);
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		IsNonNTWindows = true;

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			out.append("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				out.append("OSR2 " );
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			out.append("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				out.append( "SE " );
        } 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			out.append("Microsoft Windows Me ");

		break;

	case VER_PLATFORM_WIN32s:

		IsNonNTWindows = true;
		out.append("Microsoft Win32s ");
		break;
   }
}

//! Notifies the device, that it has been resized
void CIrrDeviceWin32::OnResized()
{
	Resized = true;
}

//! Sets if the window should be resizeable in windowed mode.
void CIrrDeviceWin32::setResizeAble(bool resize)
{
	if (!getVideoDriver() || FullScreen)
		return;

	LONG style = WS_POPUP;

	if (!resize)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	else
		style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX;

	if (!SetWindowLong(HWnd, GWL_STYLE, style))
		os::Printer::log("Could not change window style.");

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = getVideoDriver()->getScreenSize().Width;
	clientSize.bottom = getVideoDriver()->getScreenSize().Height;

	AdjustWindowRect(&clientSize, style, FALSE);

	s32 realWidth = clientSize.right - clientSize.left;
	s32 realHeight = clientSize.bottom - clientSize.top;

	s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	SetWindowPos(HWnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight, 
		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);
}


#ifdef IRRLICHT_EXPORTS
#define IRRLICHT_API __declspec(dllexport)
#else
#define IRRLICHT_API __declspec(dllimport)
#endif

IRRLICHT_API IrrlichtDevice* createDevice(video::EDriverType driverType,
										  const core::dimension2d<s32>& windowSize,
										  u32 bits, bool fullscreen, 
										  bool stenticbuffer, IEventReceiver* res,
										  const wchar_t* version)
{
	return new CIrrDeviceWin32(driverType, windowSize, bits, 
		fullscreen, stenticbuffer, res, version);
}


} // end namespace 

#endif // WIN32

