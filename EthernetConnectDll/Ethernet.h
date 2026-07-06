#pragma once

#include <winsock2.h> 
#include <WS2tcpip.h>
#include <windows.h>
#include <list>
#include <string>

#define	WM_PACKET	(WM_USER+0x1001)
#define	CAM_W	1024
#define	CAM_H	640

#ifndef MAX_SEND_SIZE
#ifdef SPACETALK
#define MAX_SEND_SIZE 	(0xFF)
#define	MAX_DATA_LEN	(200)
#else
#define MAX_SEND_SIZE 	(1444)
#define	MAX_DATA_LEN	(1400)
#endif // SPACETALK
#endif
#define		RBT_PROTOCOL_CATEGORY_WORKING		( 0x00 )

#define		RBT_PROTOCOL_RESET					( 0x01 )
#define		RBT_PROTOCOL_ISCONNECTED			( 0x02 )
#define		RBT_PROTOCOL_LEDON					( 0x03 )
#define		RBT_PROTOCOL_LEDOFF					( 0x04 )
#define		RBT_PROTOCOL_READY					( 0x05 )
#define		RBT_PROTOCOL_ENABLE					( 0x06 )
#define		RBT_PROTOCOL_DISABLE				( 0x07 )
#define		RBT_PROTOCOL_SETMODE				( 0x08 )
#define		RBT_PROTOCOL_HITPOS_XY				( 0x09 )
#define		RBT_PROTOCOL_POSDIR					( 0x0A )
#define		RBT_PROTOCOL_CAMERAFRAME			( 0x0B )
#define		RBT_PROTOCOL_ANOTHER				( 0x0C )
#define		RBT_PROTOCOL_RES_CHANGE				( 0x0D )	/*카메라에 작동 명령을 전송 / 상태요청*/
#define		RBT_PROTOCOL_COMMAND				( 0x0E )	/*카메라에 작동 명령을 전송 / 상태요청*/
#define		RBT_PROTOCOL_FINDPOS				( 0x0F )	/*각종 좌표 전송요청*/

#define		RBT_PROTOCOL_IMAGEDUMP				( 0x10|0x80 )
#define		RBT_PROTOCOL_VDMA_DATA				( 0x20|0x80 )	/*카메라의 현재프레임 요청*/

#define		RBT_PROTOCOL_FILE_DEVICE_TO_ZYNQ	( 0x30|0x80 )
#define		RBT_PROTOCOL_FILE_ZYNQ_TO_DEVICE	( 0x40|0x80 )

#define		RBT_PROTOCOL_PROFILE_DATA			( 0x50|0x80 )	/*카메라의 현재프레임 프로파일 요청*/

#define		RBT_GAMEMODE_UNDEFINED				( 0xFF )
#define		RBT_GAMEMODE_SOCCER					( 0 )
#define		RBT_GAMEMODE_BASEBALL				( 1 )
#define		RBT_GAMEMODE_HITSCREEN				( 2 )
#define		RBT_GAMEMODE_SINGLE_HITSCREEN		( 3 )
#define		RBT_GAMEMODE_HOCKEY					( 4 )

#define		RBT_PRIORITY_STANDARD				( 0 )
#define		RBT_PRIORITY_PERFORMANCE			( 1 )

#define		RBT_COMMAND_REST					( 0x00 )
#define		RBT_COMMAND_SETROI					( 0x01 )
#define		RBT_COMMAND_SETEXP					( 0x02 )
#define		RBT_COMMAND_SETDGN					( 0x03 )
#define		RBT_COMMAND_SETAGN					( 0x04 )
#define		RBT_COMMAND_LEDON					( 0x05 )
#define		RBT_COMMAND_LEDOFF					( 0x06 )
#define		RBT_COMMAND_READY					( 0x07 )
#define		RBT_COMMAND_ENABLE					( 0x08 )
#define		RBT_COMMAND_TRACK					( 0x09 )
#define		RBT_COMMAND_SET_SPI					( 0x0A )
#define		RBT_COMMAND_SET_ROI					( 0x0B )
#define		RBT_COMMAND_THRESHOLD				( 0x0C )
#define		RBT_COMMAND_DTHRESHOLD				( 0x0D )
#define		RBT_COMMAND_FULLSCREEN				( 0x0E )
#define		RBT_COMMAND_GETROI					( 0x11 )
#define		RBT_COMMAND_GETEXP					( 0x12 )
#define		RBT_COMMAND_GETDGN					( 0x13 )
#define		RBT_COMMAND_GETAGN					( 0x14 )
#define		RBT_COMMAND_GETLED					( 0x15 )
#define		RBT_COMMAND_GETSTATUS				( 0x17 )
#define		RBT_COMMAND_SETSTATUS				( 0x18 )
#define		RBT_COMMAND_ROI_UP					( 0x19 )
#define		RBT_COMMAND_ROI_DN					( 0x1A )
#define		RBT_COMMAND_GET_MARKER_POS			( 0x1B )

#define		RBT_STATUS_ANALYSIS_BEGIN			( 0x50 )
#define		RBT_STATUS_ANALYSIS_END				( 0x51 )

#pragma pack(push,1)
#ifdef SPACETALK
#define	PACK_DATA_OFFSET	(2)
#define	VDMA_DATA_OFFSET	(13)
#define	FILE_DATA_OFFSET	(18)
#define	CMND_DATA_OFFSET	(35)
struct	S_PACKET
{
	union
	{
		char					packetBuffer[MAX_SEND_SIZE];
		struct
		{
			unsigned char		_protocol;
			unsigned char		_length;
			char				_data[MAX_SEND_SIZE-PACK_DATA_OFFSET];
		};
		struct
		{
			unsigned char		protocolV;
			unsigned char		lengthV;
			unsigned char		cameraIdxV;
			unsigned short		imageWV;
			unsigned short		imageHV;
			unsigned int		offsetV;
			unsigned short		frameNoV;
			char				dataV[MAX_SEND_SIZE-VDMA_DATA_OFFSET];
		};
		struct
		{
			unsigned char		protocolF;
			unsigned char		lengthF;
			char				filename[12];
			unsigned int		offsetF;
			char				dataF[MAX_SEND_SIZE-FILE_DATA_OFFSET];
		};
		struct
		{
			unsigned char		protocolC;				// most "RBT_PROTOCOL_COMMAND"
			unsigned char		lengthC;				// most 35.
			unsigned char		command;
			unsigned __int32	uParam0;				// unsigned param 0.
			unsigned __int32	uParam1;				// unsigned param 1.
			unsigned __int32	uParam2;				// unsigned param 2.
			unsigned __int32	uParam3;				// unsigned param 3.
			__int32				sParam0;				// signed param 0.
			__int32				sParam1;				// signed param 1.
			__int32				sParam2;				// signed param 2.
			__int32				sParam3;				// signed param 3.
			char				dataC[MAX_SEND_SIZE-CMND_DATA_OFFSET];//unused.
		};
	};
};
#else	// ~SPACETALK
#define	PACK_DATA_OFFSET	(8)
#define	VDMA_DATA_OFFSET	(28)
#define	FILE_DATA_OFFSET	(24)
#define	CMND_DATA_OFFSET	(44)
struct	S_PACKET
{
	union
	{
		char					packetBuffer[MAX_SEND_SIZE];
		struct
		{
			unsigned __int32	_protocol;
			unsigned __int32	_length;				// most > 8
			char				_data[MAX_SEND_SIZE-PACK_DATA_OFFSET];
		};
		struct
		{
			unsigned __int32	protocolV;
			unsigned __int32	lengthV;				// most > 28
			unsigned __int32	cameraIdxV;
			unsigned __int32	imageWV;
			unsigned __int32	imageHV;
			unsigned __int32	offsetV;
			unsigned __int32	frameNoV;
			char				dataV[MAX_SEND_SIZE-VDMA_DATA_OFFSET];
		};
		struct
		{
			unsigned __int32	protocolF;
			unsigned __int32	lengthF;				// most > 24
			char				filename[12];
			unsigned __int32	offsetF;
			char				dataF[MAX_SEND_SIZE-FILE_DATA_OFFSET];
		};
		struct
		{
			unsigned __int32	protocolC;				// most "RBT_PROTOCOL_COMMAND"
			unsigned __int32	lengthC;				// most 44.
			unsigned __int32	command;
			unsigned __int32	uParam0;				// unsigned param 0.
			unsigned __int32	uParam1;				// unsigned param 1.
			unsigned __int32	uParam2;				// unsigned param 2.
			unsigned __int32	uParam3;				// unsigned param 3.
			__int32				sParam0;				// signed param 0.
			__int32				sParam1;				// signed param 1.
			__int32				sParam2;				// signed param 2.
			__int32				sParam3;				// signed param 3.
			char				dataC[MAX_SEND_SIZE-CMND_DATA_OFFSET];//unused.
		};
	};
};
#endif // SPACETALK
#pragma pack(pop)

typedef	int	(*event_callback)(void*);

#define	_DELETE(a)	{if(a!=NULL) {delete[]a; a = NULL;}}
class CEthernet
{
protected:
	bool				bInit, bConnected;
	HANDLE				hThread;
	WSADATA				wsaData;
	SOCKADDR_IN			serveraddr, clientaddr;

	unsigned __int32	eventMsg;
	unsigned short		Init_Server	(const char* szIP, unsigned short Port );
	unsigned short		Init_Client	( const char* szIP, unsigned short Port );
	void				createThread	( );
public:
	event_callback connectFunc;
	event_callback receiveFunc;
	event_callback acceptFunc;
	event_callback closeFunc;
	event_callback writeFunc;
	void* connectArg;
	void* receiveArg;
	void* acceptArg;
	void* closeArg;
	void* writeArg;

	bool	enableCommand;
	static	std::list<std::string>	listMessages;
	WSAEVENT			listen_event;
	SOCKET				listen_sock, client_sock, accepted;
	unsigned __int64	recvCnt;
	CEthernet(void);
	virtual ~CEthernet(void);

	unsigned short	Init	( HWND hwnd, const char* szIP, unsigned short Port = 0, unsigned __int32 msg = 0xffffffff );
	void			Play	( void );
	void			Free	( void );

	void			setConnectEvent	( event_callback func, void* param );
	void			setReceiveEvent	( event_callback func, void* param );
	void			setAcceptEvent	( event_callback func, void* param );
	void			setCloseEvent	( event_callback func, void* param );
	void			setWriteEvent	( event_callback func, void* param );

	unsigned short	Acpt		( void );
	void			Send		( void* pHead, void* pTail, void* pData, int nSize = -1 );
	void			SkippedSend	( void* pHead, void* pTail, void* pData, int nSize );
	void			Recv		( void* pData, int* size = NULL );

	__inline bool	isInit		( void ){return bInit;}
	__inline void	isInit		( bool a ){bInit = a;}
	__inline bool	isConnected	( void ){return bConnected;}
	__inline void	isConnected	( bool a ){bConnected = a;}
//	virtual void	Parsing	( void* pData, int nLength, int nMode =0)				= 0;
//	virtual	void	OnPacket( HWND hwnd, WPARAM wParam, LPARAM lParam ) = 0;
};
