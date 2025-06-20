// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_IRR_DEVICE_WIN32_H_INCLUDED__
#define __C_IRR_DEVICE_WIN32_H_INCLUDED__

#ifdef WIN32

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include "IGUIEnvironment.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace irr
{

	class CIrrDeviceWin32 : public CIrrDeviceStub, video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceWin32(video::EDriverType deviceType, 
			const core::dimension2d<s32>& windowSize, u32 bits,
			bool fullscreen, bool stencilbuffer, IEventReceiver* receiver,
			const wchar_t* version);

		//! destructor
		virtual ~CIrrDeviceWin32();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run();

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text);

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive();

		//! presents a surface in the client area
		virtual void present(video::IImage* surface);

		//! notifies the device that it should close itself
		virtual void closeDevice();

		//! \return Returns a pointer to a list with all video modes supported
		//! by the gfx adapter.
		video::IVideoModeList* getVideoModeList();

		//! Notifies the device, that it has been resized
		void OnResized();

		//! Sets if the window should be resizeable in windowed mode.
		virtual void setResizeAble(bool resize=false);

		//! Implementation of the win32 cursor control
		class CCursorControl : public gui::ICursorControl
		{
		public:

			CCursorControl(const core::dimension2d<s32>& wsize, HWND hwnd, bool fullscreen)
				: WindowSize(wsize), IsVisible(true), InvWindowSize(0.0f, 0.0f),
					HWnd(hwnd), BorderX(0), BorderY(0)
			{
				if (WindowSize.Width!=0)
					InvWindowSize.Width = 1.0f / WindowSize.Width;

				if (WindowSize.Height!=0)
					InvWindowSize.Height = 1.0f / WindowSize.Height;

				if (!fullscreen)
				{
					BorderX = GetSystemMetrics(SM_CXDLGFRAME);
					BorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
				}
			}

			//! Changes the visible state of the mouse cursor.
			virtual void setVisible(bool visible)
			{
				IsVisible = visible;
			}

			//! Returns if the cursor is currently visible.
			virtual bool isVisible()
			{
				return IsVisible;
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<f32> &pos)
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(f32 x, f32 y)
			{
				setPosition((s32)(x*WindowSize.Width), (s32)(y*WindowSize.Height));
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<s32> &pos)
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(s32 x, s32 y)
			{
				RECT rect;
				if (GetWindowRect(HWnd, &rect))
					SetCursorPos(x + rect.left + BorderX, y + rect.top + BorderY);

				CursorPos.X = x;
				CursorPos.Y = y;
			}

			//! Returns the current position of the mouse cursor.
			virtual core::position2d<s32> getPosition()
			{
				updateInternalCursorPosition();
				return CursorPos;
			}

			//! Returns the current position of the mouse cursor.
			virtual core::position2d<f32> getRelativePosition()
			{
				updateInternalCursorPosition();
				return core::position2d<f32>(CursorPos.X * InvWindowSize.Width,
					CursorPos.Y * InvWindowSize.Height);
			}

		private:

			//! Updates the internal cursor position
			void updateInternalCursorPosition()
			{
				POINT p;
				GetCursorPos(&p);
				RECT rect;
				GetWindowRect(HWnd, &rect);
				CursorPos.X = p.x-rect.left-BorderX;
				CursorPos.Y = p.y-rect.top-BorderY;
			}

			core::position2d<s32> CursorPos;
			core::dimension2d<s32> WindowSize;
			core::dimension2d<float> InvWindowSize;
			bool IsVisible;
			HWND HWnd;

			s32 BorderX, BorderY;
		};


		//! returns the win32 cursor control
		CCursorControl* getWin32CursorControl();

	private:



		//! create the driver
		void createDriver(video::EDriverType driverType,
			const core::dimension2d<s32>& windowSize, u32 bits, bool fullscreen,
			bool stencilbuffer);

		//! switchs to fullscreen
		bool switchToFullScreen(s32 width, s32 height, s32 bits);

		void getWindowsVersion(core::stringc& version);

		void resizeIfNecessary();

		HWND HWnd;

		bool ChangedToFullScreen;
		bool FullScreen;
		bool IsNonNTWindows;
		bool Resized;
		CCursorControl* Win32CursorControl;
	};


} // end namespace irr

#endif
#endif

