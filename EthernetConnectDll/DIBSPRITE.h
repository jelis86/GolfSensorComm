// DIBSPRITE.h: interface for the _CDIBSPRITE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIBSPRITE_H__DEB8E640_7C9A_4A6E_91DF_C45B53B70266__INCLUDED_)
#define AFX_DIBSPRITE_H__DEB8E640_7C9A_4A6E_91DF_C45B53B70266__INCLUDED_

#pragma warning(disable : 4786)
#include "DIB.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		COLORKEY	0x00FF00FF

struct BITMAPINFO256 
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[256];
};

typedef	class _CDIBSPRITE  
{
protected:
	int			scanline;
	BYTE		*lpBuffer;
	BITMAPINFO*	m_pBMPINFO;
	LPDIB		m_lpDIB;
	HDC			hPicDC;
	HBITMAP		hBMP;
	HBITMAP		oldbmp;

	int			m_nx, m_ny;
	int			m_nCoordX;
	int			m_nCoordY;

public:
	static	int	s_nThreshold;
	_CDIBSPRITE();
	virtual ~_CDIBSPRITE();

public:
	BOOL Initial ( LPDIB, LPSTR Filename );
	BOOL Initial ( LPDIB, int Filename );
	BOOL CreateRenderTarget ( LPDIB, int w, int h, int BitCount = 24 );
	void Render				( int x, int y, DWORD Percent );
	void RenderCopy			( int x, int y, int sx, int sy, int dx, int dy, unsigned long rop = SRCCOPY );
	void RenderToDS			( _CDIBSPRITE* pTgt, int x, int y, int sx, int sy, int dx, int dy, unsigned long rop = SRCCOPY );
	void RenderColorKey		( int x, int y, int sx, int sy, int dx, int dy, unsigned int key, float angle );
	void RenderStretch		( int x, int y,	int w, int h, int sx, int sy, int sw, int sh, int rop = SRCCOPY, HDC tgt = NULL );
	void Undersample		( _CDIBSPRITE* pSrc );

	void Release	( void );

	void	WindowToLocal	( int* x, int* y );
	void	Rotate			( float angle, float x, float y );
	void	Save			( char* szFormat, ... );
	DWORD	GetPixel		( int x, int y );
	void	SetPixel		( int x, int y, BYTE r, BYTE g, BYTE b );

	void	RenderBox		( int x, int y, int w, int h, int nThick, DWORD StrokeColor, DWORD SolidColor, bool bFill = false );
	void	RenderLine		( int x, int y, int x1, int y1, int nStyle, int nThick, DWORD Color );
	void	RenderCircle	( int x, int y, unsigned int r, int nStyle, int nThick, DWORD Color );

	bool	CheckRect		( int x, int y );
	void	TextOut			( int x, int y, LPSTR Text );
	POINT	getCursorPos	( void );

	void	Difference		( _CDIBSPRITE* pDib, _CDIBSPRITE* pDi1 = NULL );
	void	GraterThan		( _CDIBSPRITE* pDib, _CDIBSPRITE* pThreshold );
	void	Quantization	( int Min, int Max, void* pOutput = NULL );
	void	Quantization	( _CDIBSPRITE* pDib, void* pOutput = NULL );

	void	blur	( int x0, int y0, int x1, int y1, _CDIBSPRITE* pOut );
	void	subtract( int x0, int y0, int x1, int y1, _CDIBSPRITE* pIn, _CDIBSPRITE* pOut );

	int		GetImageSize	( void );
	__inline	LPDIB			GetDIB			( void ) { return m_lpDIB;}
	__inline	byte*			GetBits			( void ) { return lpBuffer;}
	__inline	LPBITMAPINFO	GetInfomation	( void ) { return (m_pBMPINFO); }
	__inline	int				GetScanline		( void ) { return scanline; }
	__inline	POINT			GetPosition		( void )
	{
		POINT	r;
		r.x = m_nx;
		r.y = m_ny;
		return r;
	}
	HDC			GetBackDC ( void )
	{
		return hPicDC;
	}
}CDIBSPRITE, *LPDIBSPRITE;

#endif // !defined(AFX_DIBSPRITE_H__DEB8E640_7C9A_4A6E_91DF_C45B53B70266__INCLUDED_)
