// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include <string.h>
#include "CZipReader.h"
#include "os.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_ZLIB_
#ifdef _WIN32
#include "zlib\zlib.h"
#else
#include "zlib/zlib.h"
#endif // win32
#endif // compile with zlib

namespace irr
{
namespace io
{


CZipReader::CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths)
: File(file), IgnoreCase(ignoreCase), IgnorePaths(ignorePaths)
{
	#ifdef _DEBUG
	setDebugName("CZipReader");
	#endif

	if (File)
	{
		File->grab();

		// scan local headers
		while (scanLocalHeader());

		// prepare file index for binary search
		FileList.sort();
	}
}

CZipReader::~CZipReader()
{
	if (File)
		File->drop();

	s32 count = (s32)FileList.size();
}



//! splits filename from zip file into useful filenames and paths
void CZipReader::extractFilename(SZipFileEntry* entry)
{
	s32 lorfn = entry->header.FilenameLength; // length of real file name

	if (!lorfn)
		return;

	if (IgnoreCase)
		entry->zipFileName.make_lower();

	const c8* p = entry->zipFileName.c_str() + lorfn;
	
	// suche ein slash oder den anfang.

	while (*p!='/' && p!=entry->zipFileName.c_str())
	{
		--p;
		--lorfn;
	}

	bool thereIsAPath = p != entry->zipFileName.c_str();

	if (thereIsAPath)
	{
		// there is a path
		++p;
		++lorfn;
	}

	entry->simpleFileName = p;
	entry->path = "";

	// pfad auch kopieren
	if (thereIsAPath)
	{
		lorfn = (s32)(p - entry->zipFileName.c_str());
		entry->path.append(entry->zipFileName, lorfn);
	}

	if (!IgnorePaths)
		entry->zipFileName = entry->simpleFileName;
}



//! scans for a local header, returns false if there is no more local file header.
bool CZipReader::scanLocalHeader()
{
	c8 tmp[1024];

	SZipFileEntry entry;
	entry.fileDataPosition = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	File->read(&entry.header, sizeof(SZIPFileHeader));

	if (entry.header.Sig != 0x04034b50)
		return false; // local file headers end here.

	// read filename
	entry.zipFileName.reserve(entry.header.FilenameLength+2);
	File->read(tmp, entry.header.FilenameLength);
	tmp[entry.header.FilenameLength] = 0x0;
	entry.zipFileName = tmp;

	extractFilename(&entry);

	// move forward length of extra field.

	if (entry.header.ExtraFieldLength)
		File->seek(entry.header.ExtraFieldLength, true);

	// if bit 3 was set, read DataDescriptor, following after the compressed data
	if (entry.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRITOR)
	{
		// read data descriptor
		File->read(&entry.header.DataDescriptor, sizeof(entry.header.DataDescriptor));
	}

	// store position in file
	entry.fileDataPosition = File->getPos();

	// move forward length of data
	File->seek(entry.header.DataDescriptor.CompressedSize, true);

	#ifdef _DEBUG
	//os::Debuginfo::print("added file from archive", entry.simpleFileName.c_str());
	#endif

	FileList.push_back(entry);

	return true;
}



//! opens a file by file name
IReadFile* CZipReader::openFile(const c8* filename)
{
	s32 index = findFile(filename);

	if (index != -1)
		return openFile(index);

	return 0;
}



//! opens a file by index
IReadFile* CZipReader::openFile(s32 index)
{
	//0 - The file is stored (no compression)
	//1 - The file is Shrunk
	//2 - The file is Reduced with compression factor 1
	//3 - The file is Reduced with compression factor 2
	//4 - The file is Reduced with compression factor 3
	//5 - The file is Reduced with compression factor 4
	//6 - The file is Imploded
	//7 - Reserved for Tokenizing compression algorithm
	//8 - The file is Deflated
	//9 - Reserved for enhanced Deflating
	//10 - PKWARE Date Compression Library Imploding

	switch(FileList[index].header.CompressionMethod)
	{
	case 0: // no compression
		{
			File->seek(FileList[index].fileDataPosition);
			return createLimitReadFile(FileList[index].simpleFileName.c_str(), File, FileList[index].header.DataDescriptor.UncompressedSize);
		}
	case 8:
		{
  			#ifdef _IRR_COMPILE_WITH_ZLIB_
			
			u32 uncompressedSize = FileList[index].header.DataDescriptor.UncompressedSize;			
			u32 compressedSize = FileList[index].header.DataDescriptor.CompressedSize;

			void* pBuf = new c8[ uncompressedSize ];
			if (!pBuf)
			{
				os::Printer::log("Not enough memory for decompressing", FileList[index].simpleFileName.c_str(), ELL_ERROR);
				return 0;
			}

			c8 *pcData = new c8[ compressedSize ];
			if (!pcData)
			{
				os::Printer::log("Not enough memory for decompressing", FileList[index].simpleFileName.c_str(), ELL_ERROR);
				return 0;
			}

			//memset(pcData, 0, compressedSize );
			File->seek(FileList[index].fileDataPosition);
			File->read(pcData, compressedSize );
			
			// Setup the inflate stream.
			z_stream stream;
			s32 err;

			stream.next_in = (Bytef*)pcData;
			stream.avail_in = (uInt)compressedSize;
			stream.next_out = (Bytef*)pBuf;
			stream.avail_out = uncompressedSize;
			stream.zalloc = (alloc_func)0;
			stream.zfree = (free_func)0;

			// Perform inflation. wbits < 0 indicates no zlib header inside the data.
			err = inflateInit2(&stream, -MAX_WBITS);
			if (err == Z_OK)
			{
				err = inflate(&stream, Z_FINISH);
				inflateEnd(&stream);
				if (err == Z_STREAM_END)
					err = Z_OK;

				err = Z_OK;
				inflateEnd(&stream);
			}


			delete[] pcData;
			
			if (err != Z_OK)
			{
				os::Printer::log("Error decompressing", FileList[index].simpleFileName.c_str(), ELL_ERROR);
				delete [] (c8*)pBuf;
				return 0;
			}
			else
				return io::createMemoryReadFile ( pBuf, uncompressedSize, FileList[index].simpleFileName.c_str(), true);
			
			#else
			return 0; // zlib not compiled, we cannot decompress the data.
			#endif
		}
		break;
	default:
		os::Printer::log("file has unsupported compression method.", FileList[index].simpleFileName.c_str(), ELL_ERROR);
		return 0;
	};
}



//! returns count of files in archive
s32 CZipReader::getFileCount()
{
	return FileList.size();
}



//! returns data of file
const SZipFileEntry* CZipReader::getFileInfo(s32 index) const
{
	return &FileList[index];
}



//! deletes the path from a filename
void CZipReader::deletePathFromFilename(core::stringc& filename)
{
	// delete path from filename
	const c8* p = filename.c_str() + filename.size();

	// suche ein slash oder den anfang.

	while (*p!='/' && *p!='\\' && p!=filename.c_str())
		--p;

	core::stringc newName;

	if (p != filename.c_str())
	{
		++p;
		filename = p;
	}
}



//! returns fileindex
s32 CZipReader::findFile(const c8* simpleFilename)
{
	SZipFileEntry entry;
	entry.simpleFileName = simpleFilename;

	if (IgnoreCase)
		entry.simpleFileName.make_lower();

	if (IgnorePaths)
		deletePathFromFilename(entry.simpleFileName);

	s32 res = FileList.binary_search(entry);

	#ifdef _DEBUG
	if (res == -1)
	{
		for (u32 i=0; i<FileList.size(); ++i)
			if (FileList[i].simpleFileName == entry.simpleFileName)
			{
				os::Printer::log("File in archive but not found.", entry.simpleFileName.c_str(), ELL_ERROR);
				break;
			}
	}
	#endif

	return res;
}



} // end namespace io
} // end namespace irr

