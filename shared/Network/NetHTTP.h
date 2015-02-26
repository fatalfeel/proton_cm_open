//  ***************************************************************
//  NetHTTP - Creation date: 06/06/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

/*
Example of usage:

Here is the basic flow of usage:

NetHTTP myNet;

myNet.Setup("rtsoft.com", 80, "prey/getscores.php");

//also add any number of "post" data pieces that you want the .php to handle:

string anyData = "somedata";

myNet.AddPostData("name", &anyData.at[0], anyData.length();

myNet.Start();

while (1)
{
	if (myNet.GetError() != ERROR_NONE) 
	{
		//Big error, show message
	}

	if (myNet.GetState() == STATE_FINISHED)
	{
		//transaction is finished
		LogMsg("%s", GetDownloadedData()); //assuming it's a string.  Note that myNet will automatically add a null at the end.
	}

}
*/

#ifndef NetHTTP_h__
#define NetHTTP_h__

#include "util/MiscUtils.h"
#include "NetSocket.h"

class NetHTTP
{
public:
	NetHTTP();
	virtual ~NetHTTP();

	enum eState
	{
		STATE_IDLE,
		STATE_ACTIVE,
		STATE_FINISHED,
		STATE_FORWARD, //about to forward to another URL
		STATE_ABORT
	};

	enum eError
	{
		ERROR_NONE,
		ERROR_CANT_RESOLVE_URL,
		ERROR_COMMUNICATION_TIMEOUT,
		ERROR_WRITING_FILE,
		ERROR_404_FILE_NOT_FOUND
	};


	void Setup(string serverName, int port, string query);
	bool AddPostData(const string &name, const byte *pData, int len=-1);
	bool SetFileOutput(const string &fName); //call this before Start, allows you to save to a file instead of memory

	bool Start();
	
	eState GetState() {return m_state;}
	eError GetError() {return m_error;}

	int GetDownloadedBytes();
	int GetExpectedBytes() {return m_expectedFileBytes;} //0 if unknown
	const byte * GetDownloadedData();
	void Update();
	void Reset(bool bClearPostData=true); //completely clears it out so it can be used again
	
private:

	string BuildHTTPHeader();
	void FinishDownload();
	void OnError(eError e);
	int ScanDownloadedHeader(); //returns http result code

	string m_serverName; //eg, "www.rtsoft.com"
	string m_query;  //eg, "crap/index.html"
	string m_replyHeader; //store what the server sends us
	int m_port;
	NetSocket m_netSocket;
	eState m_state;
	eError m_error;
	vector<char> m_downloadData; //holds the actual file/stream of what we've got
	string m_downloadHeader;
	uint32 m_expectedFileBytes; //0 if content-length is unknown
	string m_postData; //optional, for if we want to send post data
	unsigned int m_timer; //we scan for an end of mark, we don't want to do it too often
	int m_idleTimeOutMS;
	FILE *m_pFile; //if not null, we're putting out download into this
	int m_bytesWrittenToFile;
	string m_fileName;
};

bool CheckCharVectorForString(vector<char> &v, string marker, int *pIndexOfMarkerEndPosOut=NULL);
#endif // NetHTTP_h__