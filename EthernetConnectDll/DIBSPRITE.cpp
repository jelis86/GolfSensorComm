// DIBSPRITE.cpp: implementation of the _CDIBSPRITE class.
//
//////////////////////////////////////////////////////////////////////

#include	"DIBSPRITE.h"
#include	"mathematics.h"

#pragma comment ( lib, "msimg32.lib" )
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int	_CDIBSPRITE::s_nThreshold = 16;

unsigned char	*g_pTemp = NULL;
_CDIBSPRITE::_CDIBSPRITE()
{
	m_pBMPINFO = NULL;
	lpBuffer = NULL;
}

_CDIBSPRITE::~_CDIBSPRITE()
{
	Release ();
	lpBuffer = NULL;
}

void	_CDIBSPRITE::Quantization	( int Min, int Max, void* pOutput )
{
	if ( !m_pBMPINFO )	return;
//	if ( !lpBuffer )	return;
	int	i	= 0, j = 0;
	int	h	= abs(m_pBMPINFO->bmiHeader.biHeight), w = abs(m_pBMPINFO->bmiHeader.biWidth);
	int	bbp = m_pBMPINFO->bmiHeader.biBitCount/8;
	unsigned char*	pData0	= lpBuffer;
	unsigned char*	pOut	= (pOutput == NULL)?lpBuffer:(unsigned char*)(pOutput);
	for ( i = 0; i < h; i ++ )
	{
		for ( j = 0; j < w; j ++ )
		{
			int	idx = (j + i*w)*bbp;
			int	a = pData0[idx];
			if ( a > Max )		pOut[idx] = 255;
			else if ( a < Min )	pOut[idx] = 0;
			else				pOut[idx] = 128;
		}
	}
//	memcpy128 ( pOut, g_pTemp, w*h*bbp );
}

void	_CDIBSPRITE::Quantization	( _CDIBSPRITE* pDib, void* pOutput )
{
	if ( !m_pBMPINFO )	return;
	if ( !lpBuffer )	return;
	int	i	= 0, j = 0;
	int	h	= abs(m_pBMPINFO->bmiHeader.biHeight), w = abs(m_pBMPINFO->bmiHeader.biWidth);
	int	bbp = m_pBMPINFO->bmiHeader.biBitCount/8;
	unsigned char*	pData0	= lpBuffer;
	unsigned char*	pData1	= pDib->GetBits();
	unsigned char*	pOut	= (pOutput == NULL)?lpBuffer:(unsigned char*)(pOutput);
	for ( i = 0; i < h; i ++ )
	{
		for ( j = 0; j < w; j ++ )
		{
			int	idx = (j + i*w)*bbp;
			int	a = pData0[idx];
			int	b = pData1[idx];
			int	g = (a-b);
			if ( g > 40 )		pOut[idx] = 255;
			else if ( g <-4 )	pOut[idx] = 0;
			else				pOut[idx] = 128;
		}
	}
	for ( i = 0; i < h; i ++ )
	{
		for ( j = 0; j < w; j ++ )
		{
			int	av = 0;
			int	idx = (j + i*w)*bbp;
			for ( int m = -2; m < 3; m ++ )
			{
				for ( int n = -2; n < 3; n ++ )
				{
					int	x	= __min(w-1, __max(0, j + n));
					int	y	= __min(h-1, __max(0, i + m));
					av	+= pOut[x+y*w];
				}
			}
			av /= 25;
//			if		( av < 116 )	g_pTemp[idx] = 0;
//			else if ( av > 140 )	g_pTemp[idx] = 255;
//			else					g_pTemp[idx] = 128;
			if		( av < 80 )				g_pTemp[idx] = 0;
			else if ( av > 170 )			g_pTemp[idx] = 255;
			else if ( pOut[idx] == 128 )	g_pTemp[idx] = 128;
		}
	}
	memcpy128 ( pOut, g_pTemp, w*h*bbp );
}

BOOL _CDIBSPRITE::CreateRenderTarget ( LPDIB lpDIB, int w, int h, int BitCount )
{
	m_lpDIB = lpDIB;
	scanline= ((w * 3) + 3) &~3;

	if ( BitCount == 8 )	m_pBMPINFO = (BITMAPINFO*)malloc (sizeof(BITMAPINFO256) );
	else					m_pBMPINFO = (BITMAPINFO*)malloc (sizeof(BITMAPINFO));
	ZeroMemory ( m_pBMPINFO, sizeof ( BITMAPINFO ));

	m_pBMPINFO->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	m_pBMPINFO->bmiHeader.biWidth			= w;
	m_pBMPINFO->bmiHeader.biHeight			= h;
	m_pBMPINFO->bmiHeader.biPlanes			= 1;
	m_pBMPINFO->bmiHeader.biBitCount		= BitCount;
	m_pBMPINFO->bmiHeader.biCompression		= BI_RGB;
	m_pBMPINFO->bmiHeader.biSizeImage		= 0;
	m_pBMPINFO->bmiHeader.biXPelsPerMeter	= 0;
	m_pBMPINFO->bmiHeader.biYPelsPerMeter	= 0;
	m_pBMPINFO->bmiHeader.biClrUsed			= 0;
	m_pBMPINFO->bmiHeader.biClrImportant	= 0;

	HDC	hScreenDC = m_lpDIB->GetBackDC();
	HRESULT	err = 0;
	hPicDC = CreateCompatibleDC(hScreenDC);
	if ( hPicDC == NULL )
	{
		err = GetLastError();
	}

	if ( BitCount == 8 )
	{
		BITMAPINFO256*			pBMPINFO = (BITMAPINFO256*)m_pBMPINFO;
		int	i = 0;
		int	threshold = 15;
		pBMPINFO->bmiHeader.biClrUsed			= 256;
		for ( i = 0; i < 256; i ++ )
		{
			pBMPINFO->bmiColors[i].rgbBlue	= i;
			pBMPINFO->bmiColors[i].rgbGreen	= i;
			pBMPINFO->bmiColors[i].rgbRed	= i;
			pBMPINFO->bmiColors[i].rgbReserved = 0;
		}

//		pBMPINFO->bmiColors[255].rgbBlue	= 0;
//		pBMPINFO->bmiColors[255].rgbGreen	= 0;
//		pBMPINFO->bmiColors[255].rgbRed		= 255;
//		pBMPINFO->bmiColors[254].rgbBlue	= 255;
//		pBMPINFO->bmiColors[254].rgbGreen	= 0;
//		pBMPINFO->bmiColors[254].rgbRed		= 0;

		hBMP	= CreateDIBSection ( hPicDC, (BITMAPINFO*)pBMPINFO, DIB_RGB_COLORS, (void **)&lpBuffer, NULL, 0);
		if ( !g_pTemp )	g_pTemp = new unsigned char [abs(pBMPINFO->bmiHeader.biWidth * abs(h))];
	}
	else	hBMP	= CreateDIBSection ( hPicDC, (BITMAPINFO *)m_pBMPINFO, DIB_RGB_COLORS, (void **)&lpBuffer, NULL, 0);
	oldbmp	= (HBITMAP)SelectObject( hPicDC, hBMP );
	scanline	= ((w * (BitCount/8))+3) & (~3);

	SetTextColor( hPicDC, RGB( 255,0,0));
	SetBkMode	( hPicDC, TRANSPARENT);

	return TRUE;
}

BOOL _CDIBSPRITE::Initial(LPDIB lpDIB, LPSTR Filename)
{
	BITMAPFILEHEADER	bf;
	BITMAPINFO256		bi;
	FILE*	fp = fopen ( Filename, "rb" );
	if ( fp )
	{
		m_pBMPINFO = new BITMAPINFO;
		fread ( &bf, sizeof(BITMAPFILEHEADER), 1, fp );
		fread ( &bi, sizeof(BITMAPINFO256), 1, fp );
		int	step = (sizeof(RGBQUAD)*256);
    	if ( bi.bmiHeader.biBitCount == 24 )	fseek ( fp, -step, SEEK_CUR );
		int	h = abs(bi.bmiHeader.biHeight), w = abs(bi.bmiHeader.biWidth);
		CreateRenderTarget ( lpDIB, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, bi.bmiHeader.biBitCount );
		fread ( lpBuffer, w*h*bi.bmiHeader.biBitCount/8, 1, fp );
		fclose(fp);
		memcpy ( m_pBMPINFO, &bi, sizeof(BITMAPINFO) );
//		hPicDC = CreateCompatibleDC ( m_lpDIB->GetBackDC() );
		oldbmp = (HBITMAP)SelectObject ( hPicDC, hBMP );

    	if ( bi.bmiHeader.biBitCount == 24 )	scanline = ((m_pBMPINFO->bmiHeader.biWidth * 3) + 3) &~3;
		else									scanline = ((m_pBMPINFO->bmiHeader.biWidth) + 3) &~3;
		return TRUE;
	}
	lpBuffer = NULL;
	return TRUE;
}

BOOL _CDIBSPRITE::Initial(LPDIB lpDIB, int Filename)
{
	m_lpDIB = lpDIB;
	if ( lpBuffer != NULL ) return FALSE;
	hBMP = (HBITMAP)LoadImage ( GetModuleHandle(NULL), MAKEINTRESOURCE(Filename), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
	if ( hBMP == NULL ) return FALSE;

	m_pBMPINFO = (BITMAPINFO*)malloc (sizeof(BITMAPINFO));
	GetObject(hBMP,sizeof(BITMAPINFO),m_pBMPINFO);

	int		Size = m_pBMPINFO->bmiHeader.biWidth * m_pBMPINFO->bmiHeader.biHeight * 3;

	BITMAPINFO	bmpinfo;
	ZeroMemory ( &bmpinfo, sizeof ( bmpinfo ));

	bmpinfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth			= m_pBMPINFO->bmiHeader.biWidth;
	bmpinfo.bmiHeader.biHeight			=-m_pBMPINFO->bmiHeader.biHeight;
	bmpinfo.bmiHeader.biPlanes			= 1;
	bmpinfo.bmiHeader.biBitCount		= 24;
	bmpinfo.bmiHeader.biCompression		= BI_RGB;
	bmpinfo.bmiHeader.biSizeImage		= 0;
	bmpinfo.bmiHeader.biXPelsPerMeter	= 0;
	bmpinfo.bmiHeader.biYPelsPerMeter	= 0;
	bmpinfo.bmiHeader.biClrUsed			= 0;
	bmpinfo.bmiHeader.biClrImportant	= 0;

	HBITMAP	hImage = CreateDIBSection	( m_lpDIB->GetBackDC(), (BITMAPINFO *)&bmpinfo, DIB_RGB_COLORS, (void **)&lpBuffer, NULL, 0);
	GetBitmapBits						( hBMP, Size, (LPVOID)lpBuffer );

	hPicDC = CreateCompatibleDC			( m_lpDIB->GetBackDC() );
	oldbmp = (HBITMAP)SelectObject		( hPicDC, hImage );

	scanline = ((m_pBMPINFO->bmiHeader.biWidth * 3) + 3) &~3;
	return TRUE;
}

void _CDIBSPRITE::Release()
{
	if ( !lpBuffer )	return;

	SelectObject ( hPicDC, oldbmp );
	if ( hPicDC != NULL )
	{
		DeleteDC ( hPicDC );
		hPicDC	= NULL;
	}
	DeleteObject ( hBMP );
	hBMP = (HBITMAP)INVALID_HANDLE_VALUE;

	lpBuffer			= NULL;
	if ( m_pBMPINFO)
	{
		free ( m_pBMPINFO );
		m_pBMPINFO = NULL;
	}
}

void _CDIBSPRITE::Render(int x, int y, DWORD Percent)
{
	if ( lpBuffer == NULL ) return;
	if ( hPicDC == NULL )
	{
		hPicDC = CreateCompatibleDC(m_lpDIB->GetBackDC());
		return;
	}

	m_nx = x;
	m_ny = y;

	LPBYTE	lpScreen	= m_lpDIB->GetBufferPoint ();
//	int		sscanline	= ((m_lpDIB->GetScreenInfo()->bmiHeader.biWidth * 3) + 3) &~3;
	int		sscanline	= ((m_lpDIB->GetScreenInfo()->bmiHeader.biWidth * (m_lpDIB->GetScreenInfo()->bmiHeader.biBitCount/8))+3) & (~3);
//	int		sscanline	= ((m_lpDIB->GetScreenInfo()->bmiHeader.biWidth * m_lpDIB->GetScreenInfo()->bmiHeader.biBitCount + 7 ) / 8 + 3) / 4;
	int		swidth = (int)(m_lpDIB->GetScreenInfo()->bmiHeader.biWidth);
	int		sheight = (int)(m_lpDIB->GetScreenInfo()->bmiHeader.biHeight);
	if ( sheight < 0 ) sheight = -sheight;

	int		Height = m_pBMPINFO->bmiHeader.biHeight;
	int		Width = m_pBMPINFO->bmiHeader.biWidth;
	int		r, g, b;
	int		sr, sg, sb;

	if ( Height < 0 ) Height = -Height;

	for ( int i = y; i < y + Height; i++)
	{
		if ( i < 0 ) continue;
		if ( i+1 > sheight ) return;
		for ( int j = (x*3); j < (x*3)+Width*3; j+=3)
		{
			if ( (j) < 0 ) continue;
			if ( (j+3) > swidth * 3 ) continue;

			int soffset	= (j) + i * sscanline;
			int offset	= (j-(x*3)) + (i-y) * scanline;

			DWORD	Color = (DWORD)lpBuffer[offset+0] << 0 |
							(DWORD)lpBuffer[offset+1] << 8 |
							(DWORD)lpBuffer[offset+2] << 16;

			if ( Color == COLORKEY ) continue;

			if ( Percent == 100 )	memcpy ( lpScreen + soffset, lpBuffer + offset, 3 );
			else if ( Percent < 100 )
			{
				sr = (int)lpScreen[soffset+0];
				sg = (int)lpScreen[soffset+1];
				sb = (int)lpScreen[soffset+2];

				r = (int)lpBuffer[offset+0];
				g = (int)lpBuffer[offset+1];
				b = (int)lpBuffer[offset+2];

				sr = ((r*Percent)/100) + ((sr*(100-Percent))/100);
				sg = ((g*Percent)/100) + ((sg*(100-Percent))/100);
				sb = ((b*Percent)/100) + ((sb*(100-Percent))/100);

				if ( sr > 255 ) lpScreen[soffset+0] = 255;
				else lpScreen[soffset+0] = sr;
				if ( sg > 255 ) lpScreen[soffset+1] = 255;
				else lpScreen[soffset+1] = sg;
				if ( sb > 255 ) lpScreen[soffset+2] = 255;
				else lpScreen[soffset+2] = sb;
			}else
			{
				sr = (int)lpScreen[soffset+0];
				sg = (int)lpScreen[soffset+1];
				sb = (int)lpScreen[soffset+2];

				r = (int)lpBuffer[offset+0];
				g = (int)lpBuffer[offset+1];
				b = (int)lpBuffer[offset+2];

				sr = ((r*(Percent-100))/100) + (sr);
				sg = ((g*(Percent-100))/100) + (sg);
				sb = ((b*(Percent-100))/100) + (sb);

				if ( sr > 255 ) lpScreen[soffset+0] = 255;
				else lpScreen[soffset+0] = sr;
				if ( sg > 255 ) lpScreen[soffset+1] = 255;
				else lpScreen[soffset+1] = sg;
				if ( sb > 255 ) lpScreen[soffset+2] = 255;
				else lpScreen[soffset+2] = sb;
			}
		}
	}
}

void	_CDIBSPRITE::SetPixel ( int x, int y, BYTE r, BYTE g, BYTE b )
{
	lpBuffer[x+(y * scanline) + 0] = r;
	lpBuffer[x+(y * scanline) + 1] = g;
	lpBuffer[x+(y * scanline) + 2] = b;
}

void _CDIBSPRITE::RenderToDS ( _CDIBSPRITE* pTgt, int x, int y, int sx, int sy, int dx, int dy, unsigned long rop )
{
	if ( hPicDC == NULL )
	{
		hPicDC = CreateCompatibleDC(m_lpDIB->GetBackDC());
		return;
	}
	LPBYTE	lpScreen = pTgt->GetBits();

	m_nx = x;
	m_ny = y;

	int		sscanline	= ((pTgt->GetInfomation()->bmiHeader.biWidth * (pTgt->GetInfomation()->bmiHeader.biBitCount/8))+3) & (~3);

	int		swidth		= (int)(pTgt->GetInfomation()->bmiHeader.biWidth);
	int		sheight		= (int)(pTgt->GetInfomation()->bmiHeader.biHeight);
	int		Height = m_pBMPINFO->bmiHeader.biHeight;
	int		Width = m_pBMPINFO->bmiHeader.biWidth;
	int		copyscan = 0, msx = 0, mdx = 0, msy = 0, mdy = 0;

	if ( sheight < 0 ) sheight = -sheight;
	if ( Height < 0 ) Height = -Height;

	if ( x < 0 )
	{
		sx -= x;
		x = 0;
	}
	if ( x + 1 > swidth ) return;
	if ( y < 0 )
	{
		sy -= y;
		y = 0;
	}
	if ( y + 1 > sheight ) return;

	if ( sx < 0 ) msx = 1;
	if ( dx < 0 ) mdx = 1;

	sx = sx % Width;
	dx = dx % Width;

	sy = sy % Height;
	dy = dy % Height;

	if ( msx == 1 ) sx = Width + sx;
	if ( mdx == 1 ) dx = Width + dx;

	if ( msy == 1 ) sy = Height + sy;
	if ( mdy == 1 ) dy = Height + dy;

	if ( ((dx - sx) + x) >= swidth )
	{
		dx = (swidth - x) + sx - 1;
	}

	HDC		MemDC = pTgt->GetBackDC();
	if ( dx > sx )
	{
		copyscan = (dx - sx + 1);
		if ( dy > sy )	BitBlt ( MemDC, x, y, dx - sx, dy - sy, hPicDC, sx, sy, rop);
		else
		{
			BitBlt ( MemDC, x, y, dx - sx, Height - sy - 1, hPicDC, sx, 0, rop);
		}
	}else
	{
		copyscan = (Width - sx);
		if ( dy > sy )	BitBlt ( MemDC, x, y, copyscan, dy - sy,		hPicDC,	sx, sy, rop);
		else			BitBlt ( MemDC, x, y, copyscan, Height - sy,	hPicDC, sx, 0,	rop);
		copyscan = (dx - 1);
		if ( dy > sy )	BitBlt ( MemDC, x+(Width-sx), y, copyscan, dy - sy,		hPicDC, 0, sy,	rop);
		else			BitBlt ( MemDC, x+(Width-sx), y, copyscan, Height - sy, hPicDC, 0, 0,	rop);
	}
}

void _CDIBSPRITE::Rotate			( float angle, float x, float y )
{
}

void _CDIBSPRITE::WindowToLocal	( int* x, int* y )
{
	*x = *x - m_nx;
	*y = *y - m_ny;
}

void _CDIBSPRITE::RenderColorKey	( int x, int y, int sx, int sy, int dx, int dy, unsigned int key, float angle )
{
	if ( hPicDC == NULL )
	{
		hPicDC = CreateCompatibleDC(m_lpDIB->GetBackDC());
		return;
	}
	HDC		MemDC = m_lpDIB->GetBackDC();
	SetGraphicsMode(hPicDC, GM_ADVANCED);
//	SetGraphicsMode(MemDC, GM_ADVANCED);
	SetMapMode( hPicDC, MM_ANISOTROPIC );

	m_nx = x;
	m_ny = y;
	if ( dx == 0 )	dx = m_pBMPINFO->bmiHeader.biWidth-1;
	if ( dy == 0 )	dy = m_pBMPINFO->bmiHeader.biHeight-1;
	int		swidth	= abs((int)(m_lpDIB->GetScreenInfo()->bmiHeader.biWidth));
	int		sheight	= abs((int)(m_lpDIB->GetScreenInfo()->bmiHeader.biHeight));


//	SIZE	size;
//	SIZE	size1;
//	SIZE	size2;
//	POINT	org;
//	POINT	org1;
//	POINT	org2;
//
//	BOOL	b;
//	b = SetViewportExtEx( hPicDC, dx/3, dy/3, &size1);
//	b = SetWindowExtEx( hPicDC, 1, 1, &size2 );
//	b = SetWindowOrgEx( hPicDC, 0, dy-1, &org2 );
//	b = SetViewportExtEx( hPicDC, 1,-1, &size1);
//	b = SetViewportOrgEx( hPicDC, 0,dy-1, &org1);
//	GetViewportExtEx( hPicDC, &size);
//	GetViewportOrgEx( hPicDC, &org);


	XFORM	xform;
	angle = 0.0f;
	float	SIN = fsin(angle);
	float	COS = fcos(angle);
	xform.eM11 = COS;
	xform.eM12 = SIN;
	xform.eM21 = -SIN;
	xform.eM22 = COS;
	xform.eDx = 0;//(FLOAT)dx / 2.0f;
	xform.eDy = 0;//-(FLOAT)dy / 2.0f;

	::SetWorldTransform ( hPicDC, &xform);

	TransparentBlt	( MemDC, x, y, dx, dy, hPicDC, sx, sy, dx, dy, key);
//	TransparentBlt	( MemDC, x-5, y-5, dx+5, dy+5, hPicDC, sx+5, sy+5, dx-5, dy-5, key);
//	BitBlt			( MemDC, 0, 0, dx, dy, hPicDC, 0, 0, SRCCOPY);
//	SetViewportExtEx( hPicDC, size1.cx, size1.cy, NULL);
//	SetViewportOrgEx( hPicDC, org1.x, org1.y, NULL);
//	SetWindowExtEx( hPicDC, size2.cx, size2.cy, NULL);
//	SetWindowOrgEx( hPicDC, org2.x, org2.y, NULL);
}

void _CDIBSPRITE::RenderCopy ( int x, int y, int sx, int sy, int dx, int dy, unsigned long rop )
{
	if ( hPicDC == NULL )
	{
		hPicDC = CreateCompatibleDC(m_lpDIB->GetBackDC());
		return;
	}
	LPBYTE	lpScreen = m_lpDIB->GetBufferPoint ();

	m_nx = x;
	m_ny = y;

	int		sscanline	= ((m_lpDIB->GetScreenInfo()->bmiHeader.biWidth * (m_lpDIB->GetScreenInfo()->bmiHeader.biBitCount/8))+3) & (~3);
//	int		sscanline	= ((m_lpDIB->GetScreenInfo()->bmiHeader.biWidth * m_lpDIB->GetScreenInfo()->bmiHeader.biBitCount + 7 ) / 8 + 3) / 4;

	int		swidth = (int)(m_lpDIB->GetScreenInfo()->bmiHeader.biWidth);
	int		sheight = (int)(m_lpDIB->GetScreenInfo()->bmiHeader.biHeight);
	int		Height = m_pBMPINFO->bmiHeader.biHeight;
	int		Width = m_pBMPINFO->bmiHeader.biWidth;
	int		copyscan = 0, msx = 0, mdx = 0, msy = 0, mdy = 0;

	sheight = abs(sheight);
	Height = abs(Height);

	if ( x < 0 )
	{
		sx -= x;
		x = 0;
	}
	if ( x + 1 > swidth ) return;
	if ( y < 0 )
	{
		sy -= y;
		y = 0;
	}
	if ( y + 1 > sheight ) return;

	if ( sx < 0 ) msx = 1;
	if ( dx < 0 ) mdx = 1;

	sx = sx % Width;
	dx = dx % Width;

	sy = sy % Height;
	dy = dy % Height;

	if ( msx == 1 ) sx = Width + sx;
	if ( mdx == 1 ) dx = Width + dx;

	if ( msy == 1 ) sy = Height + sy;
	if ( mdy == 1 ) dy = Height + dy;

	if ( ((dx - sx) + x) >= swidth )
	{
		dx = (swidth - x) + sx - 1;
	}

	HDC		MemDC = m_lpDIB->GetBackDC();
	if ( dx > sx )
	{
		copyscan = (dx - sx + 1);
		if ( dy > sy )	BitBlt ( MemDC, x, y, dx - sx, dy - sy, hPicDC, sx, sy, rop);
		else
		{
			BitBlt ( MemDC, x, y, dx - sx, Height - sy - 1, hPicDC, sx, 0, rop);
		}
	}else
	{
		copyscan = (Width - sx);
		if ( dy > sy )	BitBlt ( MemDC, x, y, copyscan, dy - sy,		hPicDC,	sx, sy, rop);
		else			BitBlt ( MemDC, x, y, copyscan, Height - sy,	hPicDC, sx, 0,	rop);
		copyscan = (dx - 1);
		if ( dy > sy )	BitBlt ( MemDC, x+(Width-sx), y, copyscan, dy - sy,		hPicDC, 0, sy,	rop);
		else			BitBlt ( MemDC, x+(Width-sx), y, copyscan, Height - sy, hPicDC, 0, 0,	rop);
	}

	return;
}

void	_CDIBSPRITE::RenderStretch	( int x, int y,	int w, int h, int sx, int sy, int sw, int sh, int rop, HDC tgt )
{
	HDC		MemDC = tgt == NULL ? m_lpDIB->GetBackDC() : tgt;
	SetStretchBltMode( MemDC, COLORONCOLOR);
	if ( sw < 1 )	sw = GetInfomation()->bmiHeader.biWidth;
	if ( sh < 1 )	sh = GetInfomation()->bmiHeader.biHeight;
	StretchBlt ( MemDC, x, y, w, h, hPicDC, sx, sy,	sw, sh, rop);
}

DWORD _CDIBSPRITE::GetPixel ( int x, int y )
{
	DWORD ret = 0UL;

	if ( x < 0 || y < 0 || x > m_pBMPINFO->bmiHeader.biWidth-1 || y > m_pBMPINFO->bmiHeader.biHeight-1 )
	{
		return COLORKEY;
	}

	DWORD	b = *(lpBuffer + (x*3) + y * scanline + 0);
	DWORD	g = *(lpBuffer + (x*3) + y * scanline + 1)<<8;
	DWORD	r = *(lpBuffer + (x*3) + y * scanline + 2)<<16;

	ret = r|g|b;

	return ret;
}

void		_CDIBSPRITE::RenderLine	( int x, int y, int x1, int y1, int nStyle, int nThick, DWORD Color )
{
	POINT	Pos2D;
	HPEN pen = CreatePen	( nStyle, nThick, Color);
	HPEN old = (HPEN)SelectObject( GetBackDC(), pen );
	MoveToEx( GetBackDC(), x, y, &Pos2D );
	LineTo( GetBackDC(), x1, y1 );
	SelectObject( GetBackDC(), old );
	DeleteObject( pen );
}

void	_CDIBSPRITE::RenderCircle( int x, int y, unsigned int r, int nStyle, int nThick, DWORD Color )
{
	LOGBRUSH	Brush;
	Brush.lbStyle = BS_HOLLOW;

	HPEN	pen = CreatePen	( nStyle, nThick, Color);
	HPEN	old = (HPEN)SelectObject( GetBackDC(), pen );
	HBRUSH	brs	= CreateBrushIndirect( &Brush );
	HBRUSH	olb = (HBRUSH)SelectObject( GetBackDC(), brs );
	Ellipse( GetBackDC(), x-r, y-r, x+r, y+r );
	SelectObject( GetBackDC(), old );
	SelectObject( GetBackDC(), olb );
	DeleteObject( pen );
	DeleteObject( brs );
}

void	_CDIBSPRITE::RenderBox	( int x, int y, int w, int h, int nThick, DWORD StrokeColor, DWORD SolidColor, bool bFill )
{
	LOGBRUSH	Brush;
	Brush.lbStyle = (!bFill)?BS_HOLLOW:BS_SOLID;
	Brush.lbColor = SolidColor;

	HPEN	pen = CreatePen	( 0, nThick, StrokeColor);
	HPEN	old = (HPEN)SelectObject( GetBackDC(), pen );
	HBRUSH	brs	= CreateBrushIndirect( &Brush );
	HBRUSH	olb = (HBRUSH)SelectObject( GetBackDC(), brs );
	Rectangle ( GetBackDC(), x, y, x+w, y+h );
	SelectObject( GetBackDC(), old );
	SelectObject( GetBackDC(), olb );
	DeleteObject( pen );
	DeleteObject( brs );
}

void _CDIBSPRITE::TextOut(int x, int y, LPSTR Text)
{
	int	tab[6] = {4, 8, 12, 16, 24, 32};
	::TabbedTextOut ( hPicDC, x, y, Text, strlen(Text), 6, tab, 0);
//	::TextOut ( hPicDC, x, y, Text, strlen(Text));
}

POINT	_CDIBSPRITE::getCursorPos	( void )
{
	POINT	pos;
	GetCursorPos(&pos);
	ScreenToClient(m_lpDIB->GetActiveHwnd(), &pos );
	pos.x -= m_nx;
	pos.y -= m_ny;
	return pos;
}

bool _CDIBSPRITE::CheckRect	( int x, int y )
{
	if ( x < 0 )
	{
		POINT pos;
		::GetCursorPos(&pos);
		::ScreenToClient ( m_lpDIB->GetActiveHwnd(), &pos );
		x = pos.x;
		y = pos.y;
	}
	if ( x < m_nx )	return false;
	if ( y < m_ny )	return false;
	if ( x > m_nx+m_pBMPINFO->bmiHeader.biWidth )		return false;
	if ( y > m_ny+abs(m_pBMPINFO->bmiHeader.biHeight))	return false;
	return true;
}

void	_CDIBSPRITE::GraterThan		( _CDIBSPRITE* pDib, _CDIBSPRITE* pThreshold )
{
	if ( !m_pBMPINFO )	return;
	int				i	= 0, j = 0;
	int				h	= abs(m_pBMPINFO->bmiHeader.biHeight), w = abs(m_pBMPINFO->bmiHeader.biWidth);
	unsigned char*	pData0	= pDib->lpBuffer;
	unsigned char*	pThres	= pThreshold->lpBuffer;
	for ( i = 0; i < h; i ++ )
	{
		for ( j = 0; j < w; j ++ )
		{
			int	idx = (j + i*w);
			float	a = pData0	[idx];
			float	b = lpBuffer[idx];
//			if ( fabs(a-b) < 35 )	continue;
//			a	/= 512.0f;
//			b	/= 255.0f;
//			float	c = __min(1, __max(0, 1-(1-a)*(1-b)));
//			lpBuffer[idx] = (unsigned char)(c*255.0f);
			lpBuffer[idx] = lpBuffer[idx]>pData0[idx] ? lpBuffer[idx]:pData0[idx];
		}
	}
}

void _CDIBSPRITE::Undersample		( _CDIBSPRITE* pSrc )
{
	if ( !m_pBMPINFO )	return;
//	if ( !lpBuffer )	return;
	int	i	= 0, j = 0;
	int	h	= abs(m_pBMPINFO->bmiHeader.biHeight), w = abs(m_pBMPINFO->bmiHeader.biWidth);
	int	bbp = m_pBMPINFO->bmiHeader.biBitCount/8;
	unsigned char*	pData0	= pSrc->lpBuffer;
	int				step	= UNDERSAMPLE_STEP;
	for ( i = 0; i < h; i ++ )
	{
		for ( j = 0; j < w; j ++ )
		{
			int	idx = (j + i*w)*bbp;
			int	a = 0;
			for ( int k = 0; k < step; k ++ )
			{
				for ( int l = 0; l < step; l ++ )
				{
					int	id1 = ((j*step+l) + ((i*step+k)*w*step))*bbp;
					a += pData0	[id1];
				}
			}
			lpBuffer[idx] = a/(step*step);
		}
	}
}

void _CDIBSPRITE::Difference( _CDIBSPRITE* pDib, _CDIBSPRITE* pDi1 )
{
	if ( !m_pBMPINFO )	return;
//	if ( !lpBuffer )	return;
	int	i	= 0, j = 0;
	int	h	= abs(m_pBMPINFO->bmiHeader.biHeight), w = abs(m_pBMPINFO->bmiHeader.biWidth);
	int	bbp = m_pBMPINFO->bmiHeader.biBitCount/8;
	unsigned char*	pData0	= pDi1 == NULL ? lpBuffer : pDi1->lpBuffer;
	unsigned char*	pOut	= pDib->lpBuffer;
	int				step	= UNDERSAMPLE_STEP;
	for ( i = 0; i < h; i ++ )
	{
		for ( j = 0; j < w; j ++ )
		{
			int	idx = (j + i*w)*bbp;
			int	a = pData0	[idx];
			int	b = pOut[idx];

/*			if ( pDi1 != NULL )
			{
				a = 0;
				for ( int k = 0; k < step; k ++ )
				{
					for ( int l = 0; l < step; l ++ )
					{
						int	id1 = ((j*step+l) + ((i*step+k)*w*step))*bbp;
						a += pData0	[id1];
					}
				}
				a /= step*step;
			}
			for ( int k = 0; k < step; k ++ )
			{
				for ( int l = 0; l < step; l ++ )
				{
					int	id1 = ((j*step+l) + ((i*step+k)*w*step))*bbp;
					b += pOut	[id1];
				}
			}
			b /= step*step;*/
			int	c = (a - b);
			if		( c > 10 )		lpBuffer[idx] = 255;
			else if ( c < -5 )		lpBuffer[idx] = 0;
//			else if ( c < -3 )		lpBuffer[idx] = 1;
			else					lpBuffer[idx] = 128;
		}
	}
}

void _CDIBSPRITE::Save		( char* format, ... )
{
	if ( !m_pBMPINFO )		return;
	va_list arg;
	CHAR szName[MAX_PATH];

	va_start(arg, format);
	vsprintf(szName, format, arg);
	va_end(arg);

	unsigned long			written = 0;
	BITMAPFILEHEADER		fileHeader;
	bool					bTabled = false;
	int						w		= m_pBMPINFO->bmiHeader.biWidth;
	int						h		= m_pBMPINFO->bmiHeader.biHeight;
	if ( m_pBMPINFO->bmiHeader.biBitCount == 8 )
	{
		fileHeader.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO256);
		fileHeader.bfReserved1	= 0;
		fileHeader.bfReserved2	= 0;
		fileHeader.bfSize		= abs(w*h) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO256);
		fileHeader.bfType		= 'MB';
		HANDLE	hFile = CreateFileA ( szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		WriteFile ( hFile, &fileHeader,				sizeof(BITMAPFILEHEADER),	&written, NULL );
		WriteFile ( hFile, &m_pBMPINFO->bmiHeader,	sizeof(BITMAPINFOHEADER),	&written, NULL );
		WriteFile ( hFile, &m_pBMPINFO->bmiColors,	sizeof(RGBQUAD)*256,		&written, NULL );
		WriteFile ( hFile, GetBits(),				abs(w*h),	&written, NULL );
		CloseHandle( hFile );
	}
	else
	{
		int	bytes = m_pBMPINFO->bmiHeader.biBitCount/8;

		fileHeader.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fileHeader.bfReserved1	= 0;
		fileHeader.bfReserved2	= 0;
		fileHeader.bfSize		= w*h*bytes + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fileHeader.bfType		= 'MB';
		HANDLE	hFile = CreateFileA ( szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		WriteFile ( hFile, &fileHeader,				sizeof(BITMAPFILEHEADER),	&written, NULL );
		WriteFile ( hFile, &m_pBMPINFO->bmiHeader,	sizeof(BITMAPINFOHEADER),	&written, NULL );
		WriteFile ( hFile, GetBits(),				w*h*bytes,	&written, NULL );
		CloseHandle( hFile );
	}
}

int		_CDIBSPRITE::GetImageSize	( void )
{
	int	w = abs(GetInfomation()->bmiHeader.biWidth);
	int	h = abs(GetInfomation()->bmiHeader.biHeight);
	int	b = GetInfomation()->bmiHeader.biBitCount/8;
	return w * h * b;
}

void	_CDIBSPRITE::blur		( int x0, int y0, int x1, int y1, _CDIBSPRITE* pOut )
{
	if ( x0 == x1 )	return;
	if ( y0 == y1 )	return;
	for (int i = y0; i <= y1; i++)
	{
		for (int j = x0; j <= x1; j++)
		{
			int sum = 0;
			unsigned short src[3][3];

			src[0][0] = lpBuffer[(i-1)*scanline+j-1];
			src[0][1] = lpBuffer[(i-1)*scanline+j-0];
			src[0][2] = lpBuffer[(i-1)*scanline+j+1];
			src[1][0] = lpBuffer[(i-0)*scanline+j-1];
			src[1][1] = lpBuffer[(i-0)*scanline+j-0];
			src[1][2] = lpBuffer[(i-0)*scanline+j+1];
			src[2][0] = lpBuffer[(i+1)*scanline+j-1];
			src[2][1] = lpBuffer[(i+1)*scanline+j-0];
			src[2][2] = lpBuffer[(i+1)*scanline+j+1];

			sum += src[0][0] << 0;
			sum += src[0][1] << 1;
			sum += src[0][2] << 0;
			sum += src[1][0] << 1;
			sum += src[1][1] << 2;
			sum += src[1][2] << 1;
			sum += src[2][0] << 0;
			sum += src[2][1] << 1;
			sum += src[2][2] << 0;

			pOut->lpBuffer[i*pOut->GetScanline()+j] = (unsigned char)(sum >> 4);
		}
	}
	for ( int k = 0; k < 5; k ++ )
	{
		for (int i = y0; i <= y1; i++)
		{
			for (int j = x0; j <= x1; j++)
			{
				int sum = 0;
				unsigned short src[3][3];

				src[0][0] = pOut->GetBits()[(i-1)*scanline+j-1];
				src[0][1] = pOut->GetBits()[(i-1)*scanline+j-0];
				src[0][2] = pOut->GetBits()[(i-1)*scanline+j+1];
				src[1][0] = pOut->GetBits()[(i-0)*scanline+j-1];
				src[1][1] = pOut->GetBits()[(i-0)*scanline+j-0];
				src[1][2] = pOut->GetBits()[(i-0)*scanline+j+1];
				src[2][0] = pOut->GetBits()[(i+1)*scanline+j-1];
				src[2][1] = pOut->GetBits()[(i+1)*scanline+j-0];
				src[2][2] = pOut->GetBits()[(i+1)*scanline+j+1];

				sum += src[0][0] << 0;
				sum += src[0][1] << 1;
				sum += src[0][2] << 0;
				sum += src[1][0] << 1;
				sum += src[1][1] << 2;
				sum += src[1][2] << 1;
				sum += src[2][0] << 0;
				sum += src[2][1] << 1;
				sum += src[2][2] << 0;

				pOut->lpBuffer[i*pOut->GetScanline()+j] = (unsigned char)(sum >> 4);
			}
		}
	}
}

void	_CDIBSPRITE::subtract	( int x0, int y0, int x1, int y1, _CDIBSPRITE* pIn, _CDIBSPRITE* pOut )
{
	if ( pIn != NULL )
	{
		for (int i = y0; i <= y1; i++)
		{
			for (int j = x0; j <= x1; j++)
			{
				short	sub = __max(0, lpBuffer[i*scanline+j] - pIn->lpBuffer[i*pIn->GetScanline()+j]);
				pOut->lpBuffer[i*pOut->GetScanline()+j] = sub;
			}
		}
	}
	else
	{
		int width = 3;
		int w = abs(m_pBMPINFO->bmiHeader.biWidth);
		int h = abs(m_pBMPINFO->bmiHeader.biHeight);
		for (int i = y0; i <= y1; i++)
		{
			for (int j = x0; j <= x1; j++)
			{
				short l = 0;
				short cur = lpBuffer[i*scanline+j];
				for (int k = -width; k <= width; k++)
				{
					for (int m = -width; m <= width; m++)
					{
						int x1 = _clip(0, w-1, m+j);
						int y1 = _clip(0, h-1, k+i);
						l += lpBuffer[x1+y1*scanline];
					}
				}
				l /= ((width*2+1)*(width*2+1));

				short	sub = __min(255, __max(0, l-cur));
				pOut->lpBuffer[i*pOut->GetScanline()+j] = sub;
			}
		}
	}
}
