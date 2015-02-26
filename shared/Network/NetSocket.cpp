#include "PlatformPrecomp.h"
#include "NetSocket.h"

#ifndef WINAPI
	#include <sys/types.h> 
	#include <sys/socket.h>
	#include <sys/wait.h> 
	#include <netinet/in.h> 
	#include <netdb.h> 
	#include <arpa/inet.h>

#ifdef ANDROID_NDK

#include <fcntl.h>

#elif defined(PLATFORM_BBX)
#include <fcntl.h>

#else
	#include <sys/fcntl.h>
#endif


#include <sys/ioctl.h>
#define INVALID_SOCKET  (~0)
#define rt_closesocket(x) close(x)

#if defined(RT_WEBOS_ARM) || defined(ANDROID_NDK) || defined (RTLINUX)
	#include <linux/sockios.h>
	#include <errno.h>

#elif defined (PLATFORM_FLASH)

#include <sys/sockio.h>
#include <sys/errno.h>

#else
	
	#include <sys/sockio.h>
#endif



#else

#ifndef ECONNREFUSED
	#define ECONNREFUSED            WSAECONNREFUSED
#endif

	#define rt_closesocket(x) closesocket(x)

#endif

NetSocket::NetSocket()
{
	m_socket    = INVALID_SOCKET;
    m_timeout   = 1;
    m_interrupt = 0;
    
}

NetSocket::~NetSocket()
{
	Kill();
}

#define C_NET_MAXHOSTNAME 254

void NetSocket::Kill()
{
	if (m_socket != INVALID_SOCKET)
	{
		#ifdef _DEBUG
		//LogMsg("Killed socket %d", m_socket);
		#endif
		rt_closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	m_readBuffer.clear();
	m_writeBuffer.clear();

}

int NetSocket::Init( string url, int port )
{
	Kill();
	//connect to another one

	m_idleTimer = GetSystemTimeTick();
	struct sockaddr_in sa;
	struct hostent     *hp;
	
	if ((hp= gethostbyname(url.c_str())) == NULL) 
	{
		/* do we know the host's */
#ifndef PLATFORM_BBX
		//no errno on bbx.  Wait, why am I even setting this?  Does this matter?
		errno= ECONNREFUSED;                       /* address? */
#endif
		return false;                                /* no */
	}

	memset(&sa,0,sizeof(sa));
	memcpy((char *)&sa.sin_addr,hp->h_addr,hp->h_length);     /* set address */
	sa.sin_family= hp->h_addrtype;
	sa.sin_port= htons((u_short)port);

	if ((m_socket= (int)socket(hp->h_addrtype,SOCK_STREAM,0)) < 0)     /* get socket */
		return false;

	int ret = connect(m_socket,(struct sockaddr *)&sa,sizeof sa);

    if (ret == -1)
    {
	  //um, it returns -1 when it works properly on Windows.  Docs wrong?!  Huh?!
      //  LogError("Couldn't open socket.");
      //  return false;
    }
    
#ifdef WINAPI
	//u_long iMode = 0;
	//ioctlsocket(m_socket, FIOASYNC, &iMode);
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_READ);
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_WRITE);
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_CONNECT);
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_OOB);
    
#else
	fcntl (m_socket, F_SETFL, O_NONBLOCK);
	
#endif
	
    return true;
}

bool NetSocket::InitHost( int port, int connections )
{
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));

	sa.sin_family = PF_INET;             
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(port);          
	m_socket = (int)socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET )
	{
		LogMsg("socket command: INVALID_SOCKET");
		return false;
	}

	//u_long arg = 1;
	
	
	//ioctlsocket(m_socket, FIONBIO, &arg);

	/* bind the socket to the internet address */
	if (::bind(m_socket, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR) 
	{
		rt_closesocket(m_socket);
		LogMsg("bind: INVALID_SOCKET");
		return false;
	}


#ifdef WINAPI

	//u_long iMode = 0;
	//ioctlsocket(m_socket, FIOASYNC, &iMode);
    ULONG NonBlock;
	
	NonBlock = 1;
	if (ioctlsocket(m_socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
	{
		LogError("ioctlsocket() failed \n");
		return false;
	}
	
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_READ); 
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_WRITE); 
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_CONNECT); 
	WSAAsyncSelect(m_socket, GetForegroundWindow(), WM_USER+1, FD_OOB); 


#else
	//int x;
	//x=fcntl(m_socket,F_GETFL,0);
	//fcntl(m_socket,F_SETFL,x | O_NONBLOCK);
	fcntl(m_socket, F_SETFL, O_NONBLOCK);
	

#endif


	listen(m_socket, connections); 
	return true;
}


void NetSocket::SetSocket( int socket )
{
	Kill();
	m_socket    = socket;
	m_idleTimer = GetSystemTimeTick();
#ifndef WINAPI
	fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif

}

void NetSocket::SetTimeout(unsigned int seconds)
{
    m_timeout = seconds;
}

void NetSocket::SetInterrupt(int value)
{
    m_interrupt = value;
}

void NetSocket::Update()
{	
	UpdateRead();
	UpdateWrite();
}

void NetSocket::UpdateRead()
{
	if (m_socket == INVALID_SOCKET)
        return;
		
	vector <char> buff;
	buff.resize(1024);
	int bytesRead;

	do
	{
		bytesRead = ::recv (m_socket, &buff[0], buff.size(), 0);
	
		if (bytesRead <= 0)
		{
			//all done
			return;
		}

		//copy it into our real lbuffer
		m_readBuffer.insert(m_readBuffer.end(), buff.begin(), buff.begin()+bytesRead);
		
#ifdef _DEBUG
		//LogMsg("Read %d bytes", bytesRead);
    #ifdef WIN32
		//LogMsg(&buff[0]);  //can't really do this, because %'s will crash it
		//OutputDebugString(&buff[0]);
		//OutputDebugString("\n");
    #endif
#endif
		m_idleTimer = GetSystemTimeTick();

	} while (bytesRead >= int(buff.size()));
}

void NetSocket::UpdateWrite()
{
	
	if (m_socket == INVALID_SOCKET || m_writeBuffer.empty())
        return;

	int bytesWritten = ::send (m_socket, &m_writeBuffer[0], m_writeBuffer.size(), 0);

	if (bytesWritten <= 0)
	{
		//socket probably hasn't connected yet
		return;
	}
	
    m_writeBuffer.erase(m_writeBuffer.begin(), m_writeBuffer.begin()+bytesWritten);
	
    m_idleTimer = GetSystemTimeTick();

#ifdef _DEBUG
	//LogMsg("wrote %d, %d left", bytesWritten, m_writeBuffer.size());
#endif
}

void NetSocket::Write( const string &msg )
{
	if (msg.empty())
        return;
	
    m_writeBuffer.insert(m_writeBuffer.end(), msg.begin(), msg.end());
	
    UpdateWrite();
}

/*void NetSocket::Write( char *pBuff, int len )
{
	m_writeBuffer.resize(m_writeBuffer.size()+len);
	memcpy(&m_writeBuffer[m_writeBuffer.size()-len], pBuff, len);
	
	UpdateWrite();
}*/

int NetSocket::GetIdleTimeMS()
{
	return GetSystemTimeTick()-m_idleTimer;
}

unsigned int NetSocket::Read( char* buffer, int nbytes )
{
	int             ret;
    int             total;
    bool            more;
    int				time_left;
	long			timeout         = m_timeout * 1000;
	unsigned int	current_time	= (unsigned int)GetSystemTimeTick();
	unsigned int	time_limit		= current_time + timeout;
	
    total   = 0;
    more    = true;
	
    while (more)
	{
		current_time	= (unsigned int)GetSystemTimeTick();
		time_left		= time_limit - current_time;
		
		if ((!timeout) || (time_left <= 0) || (m_interrupt))
			break;
        
		//ret = m_socket->Read((char *)buffer, nbytes);
        ret = ::recv(m_socket, buffer, nbytes, 0);
        
		if (ret > 0)
		{
			total  += ret;
			nbytes -= ret;
			buffer  = (char *)buffer + ret;
		}
        
		// If we got here and wxSOCKET_WAITALL is not set, we can leave
		// now. Otherwise, wait until we recv all the data or until there
		// is an error.
		//
		more = nbytes > 0;
    }
    
    return total;
}

unsigned int NetSocket::Write( char* buffer, int nbytes )
{
    int             ret;
    int             total;
    bool            more;
    int				time_left;
	long			timeout         = m_timeout * 1000;
	unsigned int	current_time	= (unsigned int)GetSystemTimeTick();
	unsigned int	time_limit		= current_time + timeout;
	
    total   = 0;
    more    = true;
    
	while (more)
	{
		//current_time = wxDateTime::UNow();
		//time_left = time_limit - ((current_time.GetTicks() * 1000) + current_time.GetMillisecond());
		current_time	= (unsigned int)GetSystemTimeTick();
		time_left		= time_limit - current_time;
		
		if ((!timeout) || (time_left <= 0) || (m_interrupt))
			break;
        
		//ret = m_socket->Write((const char *)buffer, nbytes);
        ret = ::send(m_socket, buffer, nbytes, 0);
        
		if (ret > 0)
		{
			total  += ret;
			nbytes -= ret;
			buffer  = (char *)buffer + ret;
		}
        
		// If we got here and wxSOCKET_WAITALL is not set, we can leave
		// now. Otherwise, wait until we send all the data or until there
		// is an error.
		//
		more = nbytes > 0;
	}
    
    return total;
}

