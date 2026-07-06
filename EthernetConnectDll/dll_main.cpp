#include "header.h"
#include "DIBSPRITE.h"
#include <time.h>
#include <direct.h>



unsigned __int32					uStreamingMode = STREAMING_PAUSE;
unsigned char						byDispType = DISP_SOLID;
bool								bCapture = false;
CDIBSPRITE		recorded[2][64], FinalRecorded[64];
CDIBSPRITE		rt[2], tmpRT[2];
int				camIdx = 0;
int				frameNo = 0;
string			SavePath;
string			RootSavePath = "NULL";
string			FolderName = "NULL";
byte sendData[64][640 * 480];
CEthernet ethernet;
CDIB	  g_cDIB;
bool	  bStatus = false;
bool	  bSave = false;


char* szVideoName = "";




int nWidth = 640;
int nHeight = 480;
bool bThread = true;


extern "C"
{

}
void CropImage(int frame)
{
	unsigned char* pSrc = recorded[0][frame].GetBits();
	int sc = recorded[0][frame].GetScanline();

	for (int i = 160; i < 348; i++)
	{
		memcpy(FinalRecorded[frame].GetBits(), pSrc + i * sc, 640);
	}
}


//byte* GetImg()
//{
//	return recorded[0]->GetBits();
//}
bool	ConnectToIP(char* szIp)
{
	e_state = E_WAIT;
	rt[0].CreateRenderTarget(&g_cDIB, 640, -480, 8);
	rt[1].CreateRenderTarget(&g_cDIB, 640, -480, 8);
	tmpRT[0].CreateRenderTarget(&g_cDIB, 640, -480, 8);
	tmpRT[1].CreateRenderTarget(&g_cDIB, 640, -480, 8);

	for (int i = 0; i < 64; i++)
	{
		recorded[0][i].CreateRenderTarget(&g_cDIB, 640, -480, 8);
		recorded[1][i].CreateRenderTarget(&g_cDIB, 640, -480, 8);
		FinalRecorded[i].CreateRenderTarget(&g_cDIB, 640, 188, 8);
	}

	char			szCommand[100];
	unsigned long	ipAddress = 0;
	memcpy(&ipAddress, szIp, sizeof(unsigned long));

	ethernet.setConnectEvent(onConnect, &ethernet);
	ethernet.setAcceptEvent(onAccept, &ethernet);
	ethernet.setReceiveEvent(onReceive, &ethernet);
	ethernet.setWriteEvent(onWrite, &ethernet);
	ethernet.setCloseEvent(onClose, &ethernet);
	sockaddr_in in_addr;

	//	unsigned long	IP = inet_pton (AF_INET, szCommand, &in_addr.sin_addr);
	unsigned long	IP = FIRST_IPADDRESS(ipAddress) | SECOND_IPADDRESS(ipAddress) << 8 | THIRD_IPADDRESS(ipAddress) << 16 | FOURTH_IPADDRESS(ipAddress) << 24;
	DWORD		dwID;
	if (ethernet.Init(NULL, szIp, 4000) != 0)
	{
		ethernet.setConnectEvent(NULL, NULL);
		ethernet.setAcceptEvent(NULL, NULL);
		ethernet.setReceiveEvent(NULL, NULL);
		ethernet.setWriteEvent(NULL, NULL);
		ethernet.setCloseEvent(NULL, NULL);
		ethernet.Free();
	}
	if (ethernet.isConnected() == false)
	{
		e_state = E_FAILED;
		return false;
	}
	e_state = E_SUCCESS;

	return true;
}
char szFolderName[30] = { 0 };
void	GetFrameData(S_PACKET* recv)
{
	if (recv->imageWV != rt[camIdx].GetInfomation()->bmiHeader.biWidth ||
		recv->imageHV != abs(rt[camIdx].GetInfomation()->bmiHeader.biHeight))
	{
		resetrenderTarget(camIdx, recv->imageWV, -recv->imageHV);
	}
	if (recv->lengthV <= VDMA_DATA_OFFSET)
	{
		S_PACKET packet;
		if (recv->cameraIdxV == 1)
		{
			packet.frameNoV = recv->frameNoV + 1;
			if (uStreamingMode == STREAMING_MOVIE)
			{

				memcpy(recorded[recv->cameraIdxV][recv->frameNoV].GetBits(), tmpRT[recv->cameraIdxV].GetBits(), tmpRT[recv->cameraIdxV].GetImageSize());
			}
			camIdx = 0;
			recv->frameNoV++;
		}
		else
		{
			if (uStreamingMode == STREAMING_MOVIE)
				memcpy(recorded[recv->cameraIdxV][recv->frameNoV].GetBits(), tmpRT[recv->cameraIdxV].GetBits(), tmpRT[recv->cameraIdxV].GetImageSize());
			camIdx = 1;
		}

		if (uStreamingMode == STREAMING_MOVIE)
		{
			if (recv->frameNoV < 64)
			{
				packet.protocolV = RBT_PROTOCOL_VDMA_DATA;
				packet.lengthV = VDMA_DATA_OFFSET;
				packet.cameraIdxV = camIdx;
				packet.offsetV = 0;
				packet.imageWV = 0;
				packet.imageHV = 0;
				packet.frameNoV = (recv->frameNoV);
				ethernet.Send(NULL, NULL, (void*)&packet, VDMA_DATA_OFFSET);
				if (packet.frameNoV == 0xFFFF)
				{
					packet.lengthV = VDMA_DATA_OFFSET;
				}
			}
			else	// 영상 전체가 끝났음. 저장할 지 물어봄.
			{
				// 폴더 자동생성해서 자동 저장
				uStreamingMode = STREAMING_MOVIE;
				e_state = E_COMPLETE;
				/*for (int i = 0; i < 64; i++)
				{
					memcpy(sendData[i], recorded[0][i].GetBits(), sizeof(640 * 480));
				}*/

				//e_state = E_WAIT;
				//WriteImpactMovie("./");
				//FindDirectory(dlg, "영상이 저장될 디렉토리를 선택하십시요.\0");
				//strcpy(szFolderName, CreateFolderName());
				//std::string strName(szFolderName);
				//std::string strFinal="";
				//if (RootSavePath == "NULL")
				//{
				//	RootSavePath = ".\\SaveData";
				//	mkdir(RootSavePath.c_str());

				//}
				//else
				//{
				//	mkdir(RootSavePath.c_str());
				//}
				//strFinal = RootSavePath + "\\" + strName;
				//if (FolderName == "NULL")
				//{
				//	strFinal = RootSavePath + "\\" + strName;
				//}
				//else
				//{
				//	strFinal = RootSavePath + "\\" + FolderName;
				//}
				//

				//char ch[100];
				//mkdir(strcpy(ch, strFinal.c_str()));

				//SavePath = strFinal;
				//strFinal = strFinal + "\\" + "ImpactMove.avi";
				//char ch1[128];
				////WriteImpactMovie(strcpy(ch1, strFinal.c_str()));
				//strcpy(szVideoName, strFinal.c_str());
				//bSave = true;
				packet._protocol = RBT_PROTOCOL_FINDPOS;
				packet._length = PACK_DATA_OFFSET;
				ethernet.Send(NULL, NULL, (void*)&packet, PACK_DATA_OFFSET);
				//free(szFolderName);
				uStreamingMode = STREAMING_READY;


			}
		}

		if (uStreamingMode != STREAMING_IMAGE && camIdx == 1)	return;

		//		camIdx = camIdx == 0 ? 1 : 0;
		if (uStreamingMode != STREAMING_MOVIE)
		{
			packet.protocolV = RBT_PROTOCOL_VDMA_DATA;
			packet.lengthV = VDMA_DATA_OFFSET;
			packet.cameraIdxV = camIdx;
			packet.offsetV = ((byDispType == DISP_BINARY) ? 2 : 0);
			packet.imageWV = 0;
			packet.imageHV = 0;
			packet.frameNoV = 0xffff;
			ethernet.Send(NULL, NULL, (void*)&packet, VDMA_DATA_OFFSET);
		}
	}
	else				// 더..더..
	{
		resetrenderTarget(camIdx, recv->imageWV, -recv->imageHV);
		S_PACKET	packet;
		packet.protocolV = RBT_PROTOCOL_VDMA_DATA;
		packet.lengthV = VDMA_DATA_OFFSET;
		packet.cameraIdxV = recv->cameraIdxV;
		packet.offsetV = 1;
		packet.imageWV = 0;
		packet.imageHV = 0;
		packet.frameNoV = recv->frameNoV;
		int	offset = 0;
		if (packet.frameNoV == 0xFFFF)
		{
			offset = 0;
		}
		memcpy(&offset, &recv->offsetV, 4);
		ethernet.Send(NULL, NULL, (void*)&packet, VDMA_DATA_OFFSET);
		if (tmpRT[camIdx].GetBits() != NULL)
		{
			if ((int)(offset + recv->lengthV - VDMA_DATA_OFFSET) < rt[packet.cameraIdxV].GetImageSize())
			{
				memcpy(tmpRT[packet.cameraIdxV].GetBits() + offset, &recv->dataV, recv->lengthV - VDMA_DATA_OFFSET);
			}
		}
		offset = 0;
	}
}
char* GetFolderName()
{
	return szFolderName;
}
int	Getstatus()
{
	return e_state;
}
int	Setstatus()
{
	e_state = E_WAIT;
	return e_state;
}
void	SetName(char* szpath, char* szfolderName)
{
	RootSavePath = szpath;
	FolderName = szfolderName;
}
void	requestFrame(unsigned __int32 frameNo)
{
	bStatus = false;
	uStreamingMode = STREAMING_MOVIE;
	S_PACKET	packet;
	packet.protocolV = RBT_PROTOCOL_VDMA_DATA;
	packet.lengthV = VDMA_DATA_OFFSET;
	packet.cameraIdxV = 0;
	packet.imageWV = 0;
	packet.imageHV = 0;
	packet.offsetV = ((byDispType == DISP_BINARY) ? 2 : 0);
	packet.frameNoV = frameNo;

	if (frameNo >= 64)
	{
		packet.frameNoV = frameNo;
	}

	ethernet.Send(NULL, NULL, (void*)&packet, VDMA_DATA_OFFSET);
}
void	requestAllStop()
{
	S_PACKET	packet;
	packet._protocol = RBT_PROTOCOL_DISABLE;
	packet._length = PACK_DATA_OFFSET;
	ethernet.Send(NULL, NULL, (void*)&packet, PACK_DATA_OFFSET);
}
int	onReceive(void* pParam)
{
	if (ethernet.isConnected() == false)	return -1;
	char		buf[8192];

	int dataLen = 8192;
	int	packedSize = MAX_DATA_LEN;
	int	dataOffset = 9;
	ethernet.Recv((void*)buf, &dataLen);
	S_PACKET* recv = (S_PACKET*)buf;
	if (recv->protocolV == RBT_PROTOCOL_VDMA_DATA)
	{
		GetFrameData(recv);
	}

	ethernet.enableCommand = true;
}
void	resetrenderTarget(int idx, int w, int h)
{
	if (w != rt[idx].GetInfomation()->bmiHeader.biWidth || abs(h) != abs(rt[idx].GetInfomation()->bmiHeader.biHeight))
	{
		rt[idx].Release();
		rt[idx].CreateRenderTarget(&g_cDIB, w, h, 8);
		tmpRT[idx].Release();
		tmpRT[idx].CreateRenderTarget(&g_cDIB, w, h, 8);
		for (int i = 0; i < 64; i++)
		{
			recorded[idx][i].Release();
			recorded[idx][i].CreateRenderTarget(&g_cDIB, w, h, 8);
		}

	}
}
void	WriteImpactMovie(char* szDir)
{
	for (int i = 0; i < 64; i++)
	{
		recorded[0][i].Save("%s\\L%02d.bmp\0", szDir, i);
		recorded[1][i].Save("%s\\R%02d.bmp\0", szDir, i);
	}
}
char* CreateFolderName()
{
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링

	return buf;
}

int	onConnect(void* pParam)
{
	e_state = E_SUCCESS;
	ethernet.isConnected(true);
	ethernet.isInit(true);
	ethernet.enableCommand = true;
	return 0;
}

int	onAccept(void* pParam)
{
	ethernet.isConnected(true);
	return 0;
}

int	onWrite(void* pParam)
{
	if (ethernet.isConnected() == false)	return -1;
	return 0;
}

int	onClose(void* pParam)
{
	if (ethernet.isConnected() == false)	return -1;

	ethernet.isConnected(false);
	int	cur = 0;
	return 0;
}
void ClearAllRenderTargets()
{
	rt[0].Release();
	rt[1].Release();
	tmpRT[0].Release();
	tmpRT[1].Release();

	for (int i = 0; i < 64; i++)
	{
		recorded[0][i].Release();
		recorded[1][i].Release();
		FinalRecorded[i].Release();
	}
}
void FreeEthernet()
{
	ClearAllRenderTargets();
	
	// 소켓 연결 종료
	ethernet.isConnected(false);

	// 이벤트 핸들러 해제
	ethernet.setConnectEvent(NULL, NULL);
	ethernet.setAcceptEvent(NULL, NULL);
	ethernet.setReceiveEvent(NULL, NULL);
	ethernet.setWriteEvent(NULL, NULL);
	ethernet.setCloseEvent(NULL, NULL);

	// 소켓 리소스 해제
	ethernet.Free();
	Sleep(100);
	// 상태 업데이트
	e_state = E_WAIT;
}

int GetImg(byte* buffer, int bufferSize)
{
	int dataSize = 640 * 480 * 64;  // 이미지 데이터 총 크기
	if (bufferSize < dataSize)  // Unity에서 전달한 버퍼 크기 확인
	{
		return -1;  // 버퍼 크기가 부족한 경우
	}

	memset(buffer, 0, dataSize);  // 버퍼 초기화

	for (int i = 0; i < 64; i++)
	{
		memcpy(buffer + i * 640 * 480, recorded[0][i].GetBits(), 640 * 480);
	}

	return dataSize;  // 복사한 데이터 크기 반환
}