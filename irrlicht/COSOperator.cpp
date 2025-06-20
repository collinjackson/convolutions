// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "COSOperator.h"
#include "IrrCompileConfig.h"

#ifdef WIN32
#include <windows.h>
#else
#include <string.h>
#endif

namespace irr
{


// constructor
COSOperator::COSOperator(const c8* osVersion)
{
	OperationSystem = osVersion;
}


//! destructor
COSOperator::~COSOperator()
{
}


//! returns the current operation system version as string.
const wchar_t* COSOperator::getOperationSystemVersion()
{
	return OperationSystem.c_str();
}


//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8* text)
{
	if (strlen(text)==0)
		return;

// Windows version
#ifdef WIN32
	if (!OpenClipboard(0) || text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
c8* COSOperator::getTextFromClipboard()
{
#ifdef WIN32
	if (!OpenClipboard(NULL))
		return 0;
	
	char * buffer = 0;

	HANDLE hData = GetClipboardData( CF_TEXT );
	buffer = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	return buffer;
#else
	return 0;
#endif
}



} // end namespace

