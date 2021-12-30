#pragma once

/*
	FileStream

	A marriage between a BinStream and a 
	memory mapped file.  Construct from a filename
	then treat it as a BinStream.

	Currently only read only.  Additional 
	constructors in the future should be added to 
	support creating file in write mode as well.
*/

#include "binreader.h"
#include "mmap.h"


class FileStream : public tvg::BinReader
{
	std::string fFileName;
	std::shared_ptr<mmap> fFileMap;

public:


	FileStream(const char* filename)
		: BinReader(nullptr, 0),
		fFileName(filename)
	{
		fFileMap = mmap::create_shared(filename);
		if ((fFileMap != nullptr) && fFileMap->isValid())
		{
			setData((uint8_t *)fFileMap->data(), fFileMap->size());
		}
	}

	void close()
	{
		if ((fFileMap != nullptr) && fFileMap->isValid())
		{
			fFileMap->close();
		}
	}
};
