//  ***************************************************************
//  FileSystemZip - Creation date: 09/25/2010
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2010 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef FileSystemZip_h__
#define FileSystemZip_h__

#include "FileSystem.h"
#include "util/unzip/zip.h"
#include "util/unzip/unzip.h"

//it's hella slow to find zips using just minizip, so we construct our own stl::map for faster lookups
struct ZipCacheEntry
{
	unz_file_pos m_unzfilepos;
};

typedef std::map<std::string, ZipCacheEntry> zipCacheMap;

//by stone
class FileSystemZip : public FileSystem
{
public:
	FileSystemZip();
	virtual ~FileSystemZip();

	bool Init_unz(std::string zipFileName);
	bool Init_zMemory(std::string NameInZip, unsigned char* buf, int size, int append); //by stone
	bool Init_unzMemory(unsigned char* buf, int size); //by stone
	
	std::vector<std::string>	GetContents();
	void						SetRootDirectory(std::string rootDir);
	//std::string				GetRootDirectory();
	
	void	CacheIndex();
	virtual byte* Get_z(int *pSizeOut);
	virtual byte* Get_unz( std::string fileName, int *pSizeOut);
	virtual StreamingInstance* GetStreaming(std::string fileName, int *pSizeOut); //pSizeOut currently always set to 0.  Returns null on fail. You must DELETE !
	virtual bool	FileExists(std::string fileName);
	virtual int		GetFileSize(std::string fileName);

private:
	unzFile			m_unzf;
	std::string		m_rootDir;
	std::string		m_zipFileName;
	zipCacheMap		m_cache;
	int				m_zip_size;
	byte*			m_compress_buf;
};

#endif // FileSystemZip_h__