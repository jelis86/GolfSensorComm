#include "Ethernet.h"
#pragma comment(lib, "ws2_32.lib") 

std::list<std::string>	CEthernet::listMessages;
bool					threadRun				= false;
bool					threadStop				= false;
CEthernet::CEthernet(void)
{
	connectFunc	= NULL;
	receiveFunc	= NULL;
	acceptFunc	= NULL;
	closeFunc	= NULL;
	writeFunc	= NULL;
	bInit		= false;
	enableCommand	= false;
}

CEthernet::~CEthernet(void)
{
}

DWORD WINAPI ethernetEvent	(LPVOID lpParameter)
{
	threadRun		= true;
	threadStop		= false;
	CEthernet* pEthernet = (CEthernet*)lpParameter;
	int	recvCnt = 0;
	char	errorMsg[256];
	while (threadRun)
	{
		unsigned long nready = WSAWaitForMultipleEvents ( 1, &pEthernet->listen_event, TRUE, 200, FALSE );
		if (nready == WSA_WAIT_FAILED)
		{
			sprintf(errorMsg, ("WSAWaitForMultipleEvents() failed, %08x\0"), nready);
			return -1;
		}
		else if (nready == WSA_WAIT_TIMEOUT)
		{
			sprintf(errorMsg, ("WSAWaitForMultipleEvents() timeout, %08x\0"), nready);
		}
		else if (nready == WSA_WAIT_IO_COMPLETION)
		{
		}
		else
		{
			unsigned long		index = WSA_WAIT_EVENT_0 + nready;
			WSANETWORKEVENTS	event_struct;
			if (index >= WSA_MAXIMUM_WAIT_EVENTS)
			{
				fprintf(stderr, "invalid event index: %d.\n", index);
				continue;
			}
			int eventRet = WSAEnumNetworkEvents(pEthernet->listen_sock, pEthernet->listen_event, &event_struct);
			if ( eventRet == SOCKET_ERROR)
			{
				fprintf(stderr, ("WSAEnumNetworkEvents() failed, %08x"), eventRet);
				continue;
			}
			if ( event_struct.lNetworkEvents & FD_ACCEPT )
			{
				if ( pEthernet->acceptFunc != NULL )	pEthernet->acceptFunc( pEthernet->acceptArg );
			}
			if ( event_struct.lNetworkEvents & FD_CLOSE )
			{
				if ( pEthernet->closeFunc != NULL )	pEthernet->closeFunc( pEthernet->closeArg );
			}
			if ( event_struct.lNetworkEvents & FD_CONNECT )
			{
				if ( pEthernet->connectFunc != NULL )	pEthernet->connectFunc( pEthernet->connectArg );
			}
			if ( event_struct.lNetworkEvents & FD_READ )
			{
				if ( pEthernet->receiveFunc != NULL )	pEthernet->receiveFunc( pEthernet->receiveArg );
			}
		}
	}
	threadStop				= true;
	return 0UL;
}

void			CEthernet::createThread	( )
{
	DWORD		dwID;
	hThread		= CreateThread ( NULL, 0, ethernetEvent, this, 0, &dwID );
}

unsigned short	CEthernet::Init_Server	( const char* szIP, unsigned short Port )
{
	int		retval = -1;
	DWORD	err		= 0;
	listen_sock		=	socket(AF_INET, SOCK_STREAM, 0); 
	if(listen_sock == INVALID_SOCKET)				return (2);

	WSAEVENT		listen_event = WSACreateEvent ();
	retval		=	WSAEventSelect ( listen_sock, listen_event, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE | FD_OOB | FD_CONNECT );

	if( retval == SOCKET_ERROR )					return (3);

	// bind() 
	SOCKADDR_IN serveraddr; 
	ZeroMemory(&serveraddr, sizeof(serveraddr)); 

	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(Port==0 ? 4900 : Port);
	serveraddr.sin_addr.s_addr	= htonl(INADDR_ANY);

	retval = bind(listen_sock, (sockaddr*)&serveraddr, sizeof(serveraddr)); 
	err = ::GetLastError();
	if( retval == SOCKET_ERROR )					return (4);

	retval = listen(listen_sock, 5); 
	if( retval == SOCKET_ERROR )					return (5);

	int		len		= sizeof(serveraddr);

	accepted	= accept ( listen_sock, (sockaddr*)&serveraddr, &len);
	createThread();
	if ( accepted == (-1) )
	{
		err = ::GetLastError();
		if ( err == (10035) )
		{
			bInit		= true;
			return (0);
		}
		closesocket (listen_sock);
		accepted	= 0;
		return unsigned short(err);
	}
	bInit		= true;
	return (0);
}

unsigned short	CEthernet::Acpt	( void )
{
	int		len		= sizeof(serveraddr);
	DWORD	err		= 0;
	accepted	= accept ( listen_sock, (sockaddr*)&serveraddr, &len);
	if ( accepted == (-1) )	return 1;
	enableCommand	= true;
	return (0);
}

unsigned short	CEthernet::Init_Client	( const char* szIP, unsigned short Port )
{
	int		retval = -1;
	DWORD	err		= 0;

	listen_sock		=	socket(AF_INET, SOCK_STREAM, 0); 
//	listen_sock		=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if(listen_sock == INVALID_SOCKET)				return (2);

	listen_event	= WSACreateEvent ();
	retval			= WSAEventSelect ( listen_sock, listen_event, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE | FD_OOB | FD_CONNECT );
	if( retval == SOCKET_ERROR )					return (3);

	SOCKADDR_IN serveraddr; 
	ZeroMemory(&serveraddr, sizeof(serveraddr)); 

	serveraddr.sin_family		= AF_INET;										
	serveraddr.sin_port			= htons(Port==0 ? 4000 : Port);									
	serveraddr.sin_addr.s_addr	= inet_addr(szIP);

	retval = connect(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)); 
	err		= GetLastError () ;
	createThread();

	if( retval == SOCKET_ERROR )
	{
		if ( err == 10035 )	return (0);
		else				return (4);
	}
	bInit			= true;
	enableCommand	= true;
	return (0);
}

unsigned short	CEthernet::Init	( HWND hwnd, const char* szIP, unsigned short Port, unsigned __int32 msg )
{
	if ( msg == 0xffffffff )	eventMsg = WM_PACKET;
	else						eventMsg = msg;

	if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0 )	return (1);

	if (szIP == NULL )	return Init_Server (szIP, Port );
	else				return Init_Client ( szIP, Port );
}

void	CEthernet::Play	( void )
{
}

void	CEthernet::Recv	( void* pData, int* size )
{
	int		retval = 0;
	if		( accepted != 0 )		retval	=	recv(accepted,		(char*)pData, *size, 0 );
	else if	( listen_sock != 0 )	retval	=	recv(listen_sock,	(char*)pData, *size, 0 );

	if ( size != NULL )	*size = retval;
	if ( retval < 1 )	return;
//	Queue.write ( pData, retval );
}

void	CEthernet::Free	( void )
{
	if ( bInit == false )	return;
	threadRun	= false;
	while ( threadStop == false )
	{
		threadRun = false;
		Sleep ( 1 );
	}
	if ( accepted != 0 )	closesocket ( accepted );
	if ( listen_sock != 0 )
	{
		int	sockerr = ::shutdown ( listen_sock, CF_BOTH );
		if( sockerr != SOCKET_ERROR ) 
		{
			fd_set readfds; 
			fd_set errorfds; 
			timeval timeout; 
			FD_ZERO( &readfds ); 
			FD_ZERO( &errorfds ); 
			FD_SET( listen_sock, &readfds ); 
			FD_SET( listen_sock, &errorfds ); 
			timeout.tv_sec  = 5; 
			timeout.tv_usec = 0; 
			::select( 1, &readfds, NULL, &errorfds, &timeout ); 
		}
		::closesocket ( listen_sock );
		listen_sock = NULL;
	}
	WSACleanup ();
	bInit		= false;
}

void	CEthernet::SkippedSend	( void* pHead, void* pTail, void* pData, int nSize )
{
	if ( enableCommand == false )	return;
	Send	( pHead, pTail, pData, nSize );
	enableCommand	= false;
}

void	CEthernet::Send	( void* pHead, void* pTail, void* pData, int nSize )
{
	int		retval = 0;
	if ( accepted != 0 )	retval	=	send(accepted,		(const char*)pData, nSize, 0);
	if ( listen_sock != 0 )	retval	=	send(listen_sock,	(const char*)pData, nSize, 0);
}

void			CEthernet::setConnectEvent	( event_callback func, void* param )
{
	connectFunc = func;
	connectArg	= param;
}

void			CEthernet::setReceiveEvent	( event_callback func, void* param )
{
	receiveFunc = func;
	receiveArg	= param;
}

void			CEthernet::setAcceptEvent	( event_callback func, void* param )
{
	acceptFunc	= func;
	acceptArg	= param;
}

void			CEthernet::setCloseEvent	( event_callback func, void* param )
{
	closeFunc	= func;
	closeArg	= param;
}

void			CEthernet::setWriteEvent	( event_callback func, void* param )
{
	writeFunc	= func;
	writeArg	= param;
}
