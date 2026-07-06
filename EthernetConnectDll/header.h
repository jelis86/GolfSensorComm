#pragma once

#ifndef HEADER_H
#define HEADER_H
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <stdio.h>
#include <Windows.h>


#include <tchar.h>
#include <string>
#include <stdlib.h>
#include "Ethernet.h"
#endif

#ifdef CREATEDLL_EXPORTS
#define MYDLL_DECLSPEC __declspec(dllexport)
#else
#define MYDLL_DECLSPEC __declspec(dllimport)
#endif

using namespace std;

#define		STREAMING_PAUSE	(0)
#define		STREAMING_IMAGE	(1)
#define		STREAMING_TRACK	(2)
#define		STREAMING_READY	(3)
#define		STREAMING_MOVIE	(4)
#define		PLAYBACK_MOVIE	(5)

#define		DISP_SOLID	(0)
#define		DISP_EDGE	(1)
#define		DISP_BINARY	(2)
#define		DISP_COUNT	(3)

#define FIRST_IPADDRESS(x)  (((x) >> 24) & 0xff)
#define SECOND_IPADDRESS(x) (((x) >> 16) & 0xff)
#define THIRD_IPADDRESS(x)  (((x) >> 8) & 0xff)
#define FOURTH_IPADDRESS(x) ((x) & 0xff)

enum eState
{
	E_SUCCESS = 0,
	E_FAILED = 1,
	E_WAIT = 2,
	E_COMPLETE,
};

eState e_state = E_WAIT;
extern "C" MYDLL_DECLSPEC bool	ConnectToIP(char* szIp);
extern "C" MYDLL_DECLSPEC void  requestFrame(unsigned __int32 frameNo = 0);
extern "C" MYDLL_DECLSPEC int	Getstatus();
extern "C" MYDLL_DECLSPEC int	Setstatus();
extern "C" MYDLL_DECLSPEC void	requestAllStop();
extern "C" MYDLL_DECLSPEC void	SetName(char* szpath, char* szfolderName);
extern "C" MYDLL_DECLSPEC char*	GetFolderName();
extern "C" MYDLL_DECLSPEC void  FreeEthernet();
extern "C" MYDLL_DECLSPEC int   GetImg(byte * buffer, int bufferSize);


bool myOpenVideo(const char* filename, int width, int height, double fps);
void myAddFrame(unsigned char* data, int width, int height);
void myCloseVideo();

void	GetFrameData(S_PACKET * recv);
void	Recv(void* pData, int* size);
void	Send(void* pHead, void* pTail, void* pData, int nSize);
char*	CreateFolderName();
void	WriteImpactMovie(char* szDir);
void	resetrenderTarget(int idx, int w, int h);
void	ClearAllRenderTargets();

int		onConnect(void* pParam);
int		onAccept(void* pParam);
int		onReceive(void* pParam);
int		onWrite(void* pParam);
int		onClose(void* pParam);