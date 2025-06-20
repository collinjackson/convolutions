// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_WRITE_FILE_H_INCLUDED__
#define __C_WRITE_FILE_H_INCLUDED__

#include <stdio.h>
#include "IWriteFile.h"
#include "irrString.h"

namespace irr
{

namespace io
{

	/*!
		Class for writing a real file to disk.
	*/
	class CWriteFile : public IWriteFile
	{
	public:

		CWriteFile(const wchar_t* fileName, bool append);
		CWriteFile(const c8* fileName, bool append);

		virtual ~CWriteFile();

		//! Reads an amount of bytes from the file.
		virtual s32 write(const void* buffer, s32 sizeToWrite);

		//! Changes position in file, returns true if successful.
		virtual bool seek(s32 finalPos, bool relativeMovement = false);

		//! Returns the current position in the file.
		virtual s32 getPos();

		//! Returns name of file.
		virtual const c8* getFileName();

		//! returns if file is open
		bool isOpen();

	private:

		//! opens the file
		void openFile(bool append);	

		core::stringc Filename;
		FILE* File;
		s32 FileSize;
	};

} // end namespace io
} // end namespace irr

#endif

