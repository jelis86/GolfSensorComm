// QUEUE.cpp: implementation of the RBQUEUE class.
//
//////////////////////////////////////////////////////////////////////

#pragma	warning	(disable : 4005)
#pragma	warning	(disable : 4312)
#pragma	warning	(disable : 4311)
#include "QUEUE.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RBQUEUE::RBQUEUE()
{
	m_pData		= NULL;
	m_nLength	= 4096;	// 데이터의 총길이.
	m_nReadPos	= 0;	// 읽는다면 읽게될 위치.
	m_nLastPos	= 0;	// 마지막 위치. 쓰게 되면 여기부터.
	m_pData		= new unsigned char [m_nLength];
}

RBQUEUE::~RBQUEUE()
{
}

int		RBQUEUE::SetSize		( int nLengh )
{
	if ( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	m_nLength = nLengh;
	m_pData	= new unsigned char [m_nLength];
	return m_nLength;
}

void	RBQUEUE::clear	( void )
{
	m_nLastPos = 0;
	m_nReadPos = 0;
}

int		RBQUEUE::write	( void* pIn, int nLength )
{
	int	nLastPos= m_nLastPos%m_nLength;

	int	nOver	= __max(0, (nLength + nLastPos) - m_nLength);	// 넘치는 길이.
	int	nHead	= nLength - nOver;
	memcpy ( (unsigned char*)m_pData + nLastPos, pIn, nHead );
	if ( nOver > 0 )	memcpy ( (unsigned char*)m_pData, (unsigned char*)pIn + nHead, nOver );

	m_nLastPos = (m_nLastPos + nLength);
	return m_nLastPos;
}

int		RBQUEUE::pop		( int nLength )
{
	int	nReadPos= m_nReadPos%m_nLength;
	int	nOver	= __max(0, (nLength + nReadPos) - m_nLength);	// 넘치는 길이.
	int	nHead	= nLength - nOver;
	m_nReadPos = (m_nReadPos + nLength);
	if ( m_nLastPos == m_nReadPos )
	{
		m_nLastPos = m_nLastPos%m_nLength;
		m_nReadPos = m_nReadPos%m_nLength;
	}
	return m_nReadPos;
}

int		RBQUEUE::read	( void* pOut, int nLength, int nOffset )
{
	if ( nOffset < 0 )	nOffset = 0;
	int	nReadPos= (m_nReadPos-nOffset+m_nLength)%m_nLength;
	int	nOver	= __max(0, (nLength + nReadPos) - m_nLength);	// 넘치는 길이.
	int	nHead	= nLength - nOver;
	memcpy ( (unsigned char*)pOut, (unsigned char*)m_pData + nReadPos, nHead );
	if ( nOver > 0 )	memcpy ( (unsigned char*)pOut + nHead, m_pData, nOver );

	return m_nReadPos;
}

int		RBQUEUE::find_r		( unsigned __int32 byFind, int range )
{
	unsigned char byData[4];
	unsigned __int32 dwData = 0;
	unsigned char*	pData = (unsigned char*)m_pData;
	int				idx = 0;
	for ( int i = m_nLastPos-1; i >= this->m_nLastPos-__max(1, (range-4)); i -- )
	{
		for ( int j = 0; j < 4; j ++ )
		{
			idx = (i+j-4 + m_nLength)%m_nLength;
			byData[j] = pData[idx];
		}
		memcpy ( &dwData, byData, 4);
		if ( dwData == byFind )	return i;
	}
	return -1;
}

int		RBQUEUE::read_r	( void* pOut, int nLength, int nOffset, bool bCheck )
{
	if ( nOffset < nLength )					nOffset = nLength;
	if ( nOffset > (m_nLastPos-m_nReadPos) )	return m_nReadPos;

	int	nReadPos= (m_nLastPos-nOffset+m_nLength)%m_nLength;
	int	nOver	= __max(0, (nLength + nReadPos) - m_nLength);	// 넘치는 길이.
	int	nHead	= nLength - nOver;
	memcpy ( (unsigned char*)pOut, (unsigned char*)m_pData + nReadPos, nHead );

	if ( nOver > 0 )	memcpy ( (unsigned char*)pOut + nHead, m_pData, nOver );

	if ( bCheck )
	{
		m_nReadPos += nLength;
	}
	return m_nReadPos;
}

int		RBQUEUE::remained	( void )
{
	return -(m_nReadPos - m_nLastPos)-1;
}

void	RBQUEUE::SetReadPos	( int a )
{
	m_nReadPos = a;
}

int		RBQUEUE::find		( unsigned char byFind, int npos, int nSize )
{
	int	nRemained = __min(nSize, remained());
	for ( int i1 = __max(0, npos); i1 < nRemained; i1 ++ )
	{
		int	i = (i1 + m_nReadPos)%m_nLength;
		if ( ((unsigned char*)(m_pData))[i] == byFind )	return i1;// + m_nReadPos;
	}
	return 8192;
}


