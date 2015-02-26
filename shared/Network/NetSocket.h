//  ***************************************************************
//  NetSocket - Creation date: 06/06/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef NetSocket_h__
#define NetSocket_h__

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

class NetSocket
{
public:
	NetSocket();
	virtual ~NetSocket();
	int     Init(string url, int port); //for clients
	bool    InitHost(int port, int connections); //for host sockets
	void    Update();
    void    SetTimeout(unsigned int seconds);
    void    SetInterrupt(int value);
	//void Write(char *pBuff, int len);
	void    Write(const string &msg);
	int     GetSocket() {return m_socket;}
	std::vector<char> & GetBuffer() {return m_readBuffer;}
	int     GetIdleTimeMS();
	void    Kill();
	void    SetSocket(int socket);
    void    UpdateRead();
	void    UpdateWrite();

    unsigned int Read( char* buffer, int nbytes );
    unsigned int Write( char* buffer, int nbytes );

protected:
	int             m_socket;
	
	std::vector<char> m_readBuffer;
	std::vector<char> m_writeBuffer;

	unsigned int    m_idleTimer; //time of last communication
    unsigned int    m_timeout;
    int             m_interrupt;
};

#endif // NetSocket_h__