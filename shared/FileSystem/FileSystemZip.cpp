#include "PlatformPrecomp.h"
#include "FileSystemZip.h"
#include "FileSystem/StreamingInstanceZip.h"

#define CASESENSITIVITY (0)

FileSystemZip::FileSystemZip()
{
	m_zip_size		= 0;
	m_compress_buf	= NULL;
	m_unzf			= NULL;
}

FileSystemZip::~FileSystemZip()
{
	if( m_compress_buf )
	{
		delete m_compress_buf;
		m_compress_buf = NULL;
	}
	
	if (m_unzf)
		unzClose(m_unzf);
}

bool FileSystemZip::Init_unz(std::string zipFileName)
{
	assert(!m_unzf && "We don't handle calling this twice yet");

	m_unzf = unzOpen(zipFileName.c_str());
	if (!m_unzf)
	{
		LogError("Cannot mount virtual file system using %s",zipFileName.c_str());
		return false;
	}
	LogMsg("Mounted zip as file system: %s", zipFileName.c_str());

	m_zipFileName = zipFileName; //save it in case we need to spawn more zip readers for streaming operations
	
	CacheIndex();
	return true; //success
}

//by stone
bool FileSystemZip::Init_zMemory(std::string NameInZip, unsigned char* buf, int size, int append)
{
	unzFile			zf;
	zip_fileinfo	zi;
	time_t			tm_t;
	struct tm*		filedate;
	int				opt_compress_level = Z_BEST_COMPRESSION; //use best compress

	if( m_compress_buf )
	{
		delete m_compress_buf;
		m_compress_buf = NULL;
	}

	m_compress_buf = new byte[size];
	
	zf = ZipOpen_Memory(m_compress_buf, size, append);
	if (!zf)
	{
		LogError("Cannot create memory using");
		return false;
	}
	LogMsg("deflate zip as memory");

	memset(&zi,0,sizeof(zip_fileinfo));
	
	tm_t		= time(NULL);
	filedate	= localtime(&tm_t);
			
	zi.tmz_date.tm_sec  = filedate->tm_sec;
	zi.tmz_date.tm_min  = filedate->tm_min;
	zi.tmz_date.tm_hour = filedate->tm_hour;
	zi.tmz_date.tm_mday = filedate->tm_mday;
	zi.tmz_date.tm_mon  = filedate->tm_mon ;
	zi.tmz_date.tm_year = filedate->tm_year;

	zipOpenNewFileInZip(zf,
						NameInZip.c_str(),
						&zi,
						NULL,
						0,
						NULL,
						0,
						NULL,
						Z_DEFLATED,
						opt_compress_level);

	zipWriteInFileInZip(zf,buf,size);
	zipCloseFileInZip(zf);
	
	m_zip_size = zipClose(zf, NULL);
	
	//m_zipFileName = zipFileName; //save it in case we need to spawn more zip readers for streaming operations
	//CacheIndex();

	return true; //success
}

bool FileSystemZip::Init_unzMemory(unsigned char* buf, int size)
{
	assert(!m_unzf && "We don't handle calling this twice yet");

	m_unzf = UnZipOpen_Memory(buf, size);
	if (!m_unzf)
	{
		LogError("Cannot mount memory using");
		return false;
	}
	LogMsg("Mounted zip as memory");

	//m_zipFileName = zipFileName; //save it in case we need to spawn more zip readers for streaming operations
	
	CacheIndex();
	return true; //success
}

void FileSystemZip::CacheIndex()
{
	assert(m_cache.empty() && "Why would you want to call this twice?");

	int				err;
	uLong			i;
	unz_global_info gi;
	ZipCacheEntry	entry;
	char			filename_inzip[512];
	unz_file_info	file_info;
	
	err = unzGetGlobalInfo (m_unzf,&gi);

	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzGetGlobalInfo \n",err);
		return;
	}
	unzGoToFirstFile(m_unzf);
	
	for(i=0;i<gi.number_entry;i++)
	{
		err = unzGetCurrentFileInfo(m_unzf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
	
		if (err!=UNZ_OK)
		{
			LogError("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			break;
		}
		
		err = unzGetFilePos(m_unzf, &entry.unzfilepos);
		if (err!=UNZ_OK)
		{
			LogError("error %d with zipfile in unzGetFilePos\n",err);
			break;
		}
		m_cache[filename_inzip] = entry;

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(m_unzf);
			if (err!=UNZ_OK)
			{
				LogError("error %d with zipfile in unzGoToNextFile\n",err);
				break;
			}
		}
	}

	LogMsg("Cache has %d files.", m_cache.size());
}

std::vector<std::string> FileSystemZip::GetContents()
{
	uLong			i;
	unz_global_info gi;
	int				err;
	char			filename_inzip[512];
	unz_file_info	file_info;
	
	std::vector<std::string> contents;

	err = unzGetGlobalInfo(m_unzf,&gi);

	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzGetGlobalInfo \n",err);
		return contents;
	}

	unzGoToFirstFile(m_unzf);

	for (i=0;i<gi.number_entry;i++)
	{
		err = unzGetCurrentFileInfo(m_unzf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		
		if (err!=UNZ_OK)
		{
			LogError("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			break;
		}
		contents.push_back(std::string(filename_inzip));

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(m_unzf);
			if (err!=UNZ_OK)
			{
				LogError("error %d with zipfile in unzGoToNextFile\n",err);
				break;
			}
		}
	}

	return contents;
}

byte* FileSystemZip::Get_z( int* pSizeOut )
{
	if( m_zip_size > 0 )
		*pSizeOut = m_zip_size;
	else
		*pSizeOut = -1;

	return m_compress_buf;
}

byte* FileSystemZip::Get_unz( std::string fileName, int *pSizeOut )
{
	int						err		= UNZ_OK;
	char					st_filename_inzip[512];
	unz_file_info			file_info;
	zipCacheMap::iterator	itor	= m_cache.find(m_rootDir+fileName);
	byte*					pBytes;

	if (itor == m_cache.end())
	{
		return NULL; //not found in this zip
		//bingo!
	}
		
	err = unzGoToFilePos(m_unzf, &itor->second.unzfilepos);
	
	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzGoToFilePos",err);
		return NULL;
	}
		
	err = unzGetCurrentFileInfo(m_unzf,&file_info,st_filename_inzip,sizeof(st_filename_inzip),NULL,0,NULL,0);

	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzGetCurrentFileInfo",err);
		return NULL;
	}
	
	//let's allocate our own memory and pass the pointer back to them.
	pBytes = new byte[file_info.uncompressed_size+1]; //the extra is because I will add a null later, helps when processing
			
	if (pBytes)
	{
		//memory allocated
		*pSizeOut =  file_info.uncompressed_size;
		pBytes[file_info.uncompressed_size] = 0;
	}   
	else
	{
		LogError("Couldn't allocate the required %d bytes to unzip into.", file_info.uncompressed_size+1);
		return NULL;
	}

	err = unzOpenCurrentFile(m_unzf);

	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzOpenCurrentFile",err);
		return NULL;
	}

	err = unzReadCurrentFile(m_unzf,pBytes,file_info.uncompressed_size);
	if (err<0)	
	{
		LogError("error %d with zipfile in unzReadCurrentFile",err);
		return NULL;

	}

	err = unzCloseCurrentFile(m_unzf);
	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzCloseCurrentFile",err);
		return NULL;
	}

	return pBytes;
}

void FileSystemZip::SetRootDirectory(std::string rootDir)
{
	m_rootDir = rootDir+"/";
}

StreamingInstance* FileSystemZip::GetStreaming( std::string fileName, int *pSizeOut )
{
	StreamingInstanceZip* pStream;
	zipCacheMap::iterator itor = m_cache.find(m_rootDir+fileName);

	if (itor == m_cache.end())
	{
		return NULL; //not found in this zip
	}

	pStream = new StreamingInstanceZip;

	if (!pStream->Init(m_zipFileName))
	{
		LogMsg("Error opening zip %s for stream", m_zipFileName.c_str());
		delete pStream;
		return NULL;
	}

	if (!m_rootDir.empty())
	{
		pStream->SetRootDirectory(m_rootDir.substr(0, m_rootDir.length()-1));
	}

	if (!pStream->OpenWithCacheEntry(&itor->second))
	{
		LogMsg("Error opening the file %s from the zip %s.", fileName.c_str(), m_zipFileName.c_str());
		delete pStream;
		return NULL;
	}
	
	return pStream;
}

bool FileSystemZip::FileExists( std::string fileName )
{
	
	zipCacheMap::iterator itor = m_cache.find(m_rootDir+fileName);

	if (itor == m_cache.end())
	{
		return NULL; //not found in this zip
	}
	return true;
}

int FileSystemZip::GetFileSize( std::string fileName )
{
	int						err = UNZ_OK;
	unz_file_info			file_info;
	char					st_filename_inzip[512];
	zipCacheMap::iterator	itor = m_cache.find(m_rootDir+fileName);

	if (itor == m_cache.end())
	{
		return -1; //not found in this zip
	}
	
	err = unzGoToFilePos(m_unzf, &itor->second.unzfilepos);

	if (err!=UNZ_OK)
	{
		LogError("error %d with unzGoToFilePos in unzGetCurrentFileInfo",err);
		return -1;
	}
	
	err = unzGetCurrentFileInfo(m_unzf,&file_info,st_filename_inzip,sizeof(st_filename_inzip),NULL,0,NULL,0);

	if (err!=UNZ_OK)
	{
		LogError("error %d with zipfile in unzGetCurrentFileInfo",err);
		return false;
	}

//	LogMsg("File is filesize %d", (int)file_info.uncompressed_size );
	return (int)file_info.uncompressed_size;
}