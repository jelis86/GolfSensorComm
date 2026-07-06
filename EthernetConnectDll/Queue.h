// QUEUE.h: interface for the CQUEUE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__17297F39_222A_46A9_ABC9_AFA67AA27FD0__INCLUDED_)
#define AFX_QUEUE_H__17297F39_222A_46A9_ABC9_AFA67AA27FD0__INCLUDED_

#include	<windows.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RBQUEUE  
{
protected:
public:
	RBQUEUE();
	virtual ~RBQUEUE();

	void*	m_pData;
	int		m_nLength;	// 데이터의 총길이.
	int		m_nReadPos;	// 읽는다면 읽게될 위치.
	int		m_nLastPos;	// 마지막 위치. 쓰게 되면 여기부터.

	int		SetSize		( int nLengh );														// 큐사이즈를 지정.
	int		write		( void* pIn, int nLength );											// 큐에 pIn을 nLength만큼 넣는다. 이때 쓰기위치가 이동.
	int		read		( void* pOut, int nLength, int offset = -1 );						// 큐에 저장된 데이터를 offset만큼 전 부터 nLength만큼 pOut에 넣는다. 읽기 위치가 이동하지 않는다.
	int		read_r		( void* pOut, int nLength, int offset = -1, bool bCheck = false );	// 큐에 저장된 데이터를 쓰기 위치를 기준으로 offset만큼 전 부터 nLength만큼 pOut에 넣는다. 읽기 위치가 이동하지 않는다.
	int		pop			( int nLength );													// 큐의 읽기위치를 nLength만큼 앞데이터를 삭제시킨다.(실제로는 읽기 위치만 이동)
	int		remained	( void );															// 읽기위치와 쓰기위치의 간격크기를 알려준다.
	int		find		( unsigned char byFind, int npos = 0, int nSize = 8192 );
	int		find_r		( unsigned __int32 byFind, int range );
	void	clear		( void );
	void	SetReadPos	( int a );
};

#endif // !defined(AFX_QUEUE_H__17297F39_222A_46A9_ABC9_AFA67AA27FD0__INCLUDED_)
