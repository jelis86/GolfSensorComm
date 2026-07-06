// DIB.h: interface for the CDIB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__F6C4C877_C21A_4D86_94B1_2AFAFB6FAF7D__INCLUDED_)
#define AFX_DIB_H__F6C4C877_C21A_4D86_94B1_2AFAFB6FAF7D__INCLUDED_

#pragma warning(disable : 4786)
#include	<Windows.h>
#include	<stdio.h>
#include	<deque>
#include	<string>
#include	"mathematics.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STR_XVC_NAME		"ADUINO ANALOG SIGNAL ANALIZER"

__inline void	memcpy128	(void* dest, void* src, unsigned __int32 ssize)
{
	memcpy ( dest, src, ssize );
/*	__asm
	{
		cld;
		mov		esi, [src];
		mov		edi, [dest];
		mov		ecx, ssize;
		push	ecx;
		shr		ecx, 4;
		jz		L_2;
L_1:
		movdqu	xmm0, [esi];
		movdqu	[edi], xmm0;
		add		esi, 16;
		add		edi, 16;
		loop	L_1;
L_2:
		pop		ecx;
		and		ecx, 15;
		rep		movsb;
	}*/
}

typedef	class _CDIB
{
private:
	int			scanline;
	int			Zscanline;
	BITMAPINFO	bmpinfo;
	HDC			hScreenDC, hMemDC, hBackupDC;
	HBITMAP		hBMPScreen, holdScreen;
	BOOL		m_bInitial;
	HWND		m_hWnd;
	DEVMODE		m_dvMode;

	int			m_nWidth, m_nHeight;
	BYTE*		lpScreen;
	DWORD		m_dwPastTime;
	DWORD		m_dwThisTime;

	double*		m_pZBuffer;
	std::deque<std::string>	strInfos;
	void		RenderTriangle2		( VectorX3* pos, DWORD color );
	void		RenderTriangleUpH	( VectorX3* pos, DWORD color );
	void		RenderTriangleDnH	( VectorX3* pos, DWORD color );
public:
	HFONT		m_hFont;
	Matrix4x4	m_mView, m_mProj, m_mVP;
	VectorX3	m_vCamPos;
	int			_3DviewportX;
	int			_3DviewportY;
	int			_3DviewportW;
	int			_3DviewportH;

	void		CreateZBuffer ( void );

	LPBYTE GetBufferPoint ( void );
	void TextOut ( int x, int y, LPSTR Text );
	void DrawText( int x, int y, int w, int h, LPSTR Text );
	void DrawText( RECT rect, LPSTR Text, unsigned int align = 0 );
	void Flip ( int x = 0, int y = 0, int w = 0, int h = 0);
	void SetPixel ( int x, int y, BYTE r, BYTE g, BYTE b);
	void SetPixel ( int x, int y, DWORD color);

	WORD  GetZPixel(int x, int y );
	DWORD GetPixel ( int x, int y );

	BOOL Initial( HWND hwnd, int Width, int Height, DWORD Formats = BI_RGB, WORD BitCount = 3, DWORD ImageSize = 0 );
	void Release ( void );
	void RestoreResolution ( WORD wParam );

	void ClearScreen ( void );

	void		RenderArrow		( VectorX3 vPos, VectorX3 D, VectorX3 N, int nStyle, int nThick, DWORD Color, char* szCaption );
	VectorX3	RenderArc		( VectorX3 vPos, VectorX3 s, VectorX3 t, int nStyle, int nThick, DWORD Color, char* szCaption, double k, unsigned long captionPos = 0 );
	void		RenderLine		( VectorX3 vMin, VectorX3 vMax, int nStyle, int nThick, DWORD Color );
	void		RenderAABB		( VectorX3 vMin, VectorX3 vMax, DWORD StrokeColor );
	void		RenderFlipBox	( VectorX3 vMin, VectorX3 vMax, DWORD StrokeColor );
	void		RenderWireBox	( VectorX3 vMin, VectorX3 vMax, DWORD StrokeColor );
	void		RenderCircle	( VectorX3 vPos, int r, int nStyle, int nThick, DWORD Color );
	void		RenderCross		( VectorX3 vPos, int w, int h, int nThick, DWORD StrokeColor );
	void		RenderBox		( int x, int y, int w, int h, int nThick, DWORD StrokeColor, DWORD SolidColor, bool bFill = false );
	void		RenderRect		( RECT rect, int nThick, DWORD StrokeColor, DWORD SolidColor, bool bFill = false );
	void		RenderCross		( int x, int y, int w, int h, int nThick, DWORD StrokeColor );
	void		RenderLine		( int x, int y, int x1, int y1, int nStyle, int nThick, DWORD Color );
	void		RenderCircle	( int x, int y, unsigned int r, int nStyle, int nThick, DWORD Color );
	void		RenderNGon		( VectorX3* pos, int nCnt, DWORD color, double scale = 1, bool bFill = false, bool bLighting = true );
	void		RenderGraph		( int x, int y, int w, int t, int border, int solid, double data );
	void		RenderTriangle	( VectorX3* pos, DWORD color, bool bLighting = true );
	void		drawGradientRect( int x, int y, int w, int h, unsigned long color0, unsigned long color1, unsigned long mode );
	void		RenderElllipse	( int x, int y, int minR, int maxR, float Rotate, unsigned long color );

	BOOL		FrameSkip		( float Frame );
	void		ResetFrameSkip	( void );

	Vector2		GetScreenPos	( VectorX3 a );
	Vector2		GetScreenPosV	( VectorX3 a );

	void		SetRenderTarget	( HDC hdc );

	HDC			GetBackDC ( void )
	{
		return hMemDC;
	}

	HWND		GetActiveHwnd	( void ) {return m_hWnd;}
	BITMAPINFO	*GetScreenInfo ( void )
	{
		return &(bmpinfo);
	}
	_CDIB();
	virtual ~_CDIB();
}CDIB, *LPDIB;

#endif // !defined(AFX_DIB_H__F6C4C877_C21A_4D86_94B1_2AFAFB6FAF7D__INCLUDED_)
