// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __S_KEY_MAP_H_INCLUDED__
#define __S_KEY_MAP_H_INCLUDED__

#include "Keycodes.h"

namespace irr
{

	//! enumeration for key actions. Used for example in the FPS Camera.
	enum EKEY_ACTION
	{
		EKA_MOVE_FORWARD = 0,
		EKA_MOVE_BACKWARD,
		EKA_STRAFE_LEFT,
		EKA_STRAFE_RIGHT,
		EKA_JUMP,
		EKA_FOCUS,
		EKA_COUNT,

		//! This value is not used. It only forces this enumeration to compile in 32 bit. 
		EKA_FORCE_32BIT = 0x7fffffff	
	};

	//! Struct storing which key belongs to which action.
	struct SKeyMap
	{
		EKEY_ACTION Action;
		EKEY_CODE KeyCode;
	};

} // end namespace

#endif

