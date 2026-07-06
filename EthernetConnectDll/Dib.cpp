// DIB.cpp: implementation of the CDIB class.
//
//////////////////////////////////////////////////////////////////////
#include "DIB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_CDIB::_CDIB()
{
	lpScreen = NULL;
	m_bInitial = FALSE;
}

_CDIB::~_CDIB()
{
	Release ();
	m_bInitial = FALSE;
}

void _CDIB::SetRenderTarget	( HDC hdc )
{
	if ( hdc == NULL )	hMemDC	= hBackupDC;
	else				hMemDC	= hdc;
}

void _CDIB::RestoreResolution ( WORD Flag )
{
}

BOOL _CDIB::Initial( HWND hwnd, int Width, int Height, DWORD Formats, WORD BitCount, DWORD ImageSize )
{
	m_nWidth = Width;
	m_nHeight = Height;

	m_pZBuffer = NULL;
	scanline= ((Width * 3) + 3) &~3;

	ZeroMemory ( &bmpinfo, sizeof ( bmpinfo ));

	bmpinfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth			= Width;
	bmpinfo.bmiHeader.biHeight			= -Height;
	bmpinfo.bmiHeader.biPlanes			= 1;
	bmpinfo.bmiHeader.biBitCount		= 24;
	bmpinfo.bmiHeader.biCompression		= BI_RGB;
	bmpinfo.bmiHeader.biSizeImage		= 0;
	bmpinfo.bmiHeader.biXPelsPerMeter	= 0;
	bmpinfo.bmiHeader.biYPelsPerMeter	= 0;
	bmpinfo.bmiHeader.biClrUsed			= 0;
	bmpinfo.bmiHeader.biClrImportant	= 0;

	hScreenDC = GetDC ( hwnd );
	hMemDC = CreateCompatibleDC(hScreenDC);
	SetGraphicsMode(hMemDC, GM_ADVANCED);
	SetGraphicsMode(hScreenDC, GM_ADVANCED);

	hBMPScreen = CreateDIBSection ( hScreenDC, (BITMAPINFO *)&bmpinfo, DIB_RGB_COLORS, (void **)&lpScreen, NULL, 0);
	holdScreen = (HBITMAP)SelectObject( hMemDC, hBMPScreen );

	SetTextColor ( hMemDC, RGB( 255,255,255));
	SetBkMode ( hMemDC, TRANSPARENT);
	ZeroMemory ( lpScreen, Width * Height * 3 );

	hBackupDC = hMemDC;

	m_hWnd = hwnd;

	// 해상도를 바꾼다.
    DEVMODE devmode;
    DISPLAY_DEVICE DisplayDevice[2];

    memset(&DisplayDevice[0],0,sizeof(DISPLAY_DEVICE));
    memset(&DisplayDevice[1],0,sizeof(DISPLAY_DEVICE));
    memset(&devmode,0,sizeof(devmode));

	// 해상도를 저장.
	//EnumDisplaySettings ( NULL, ENUM_CURRENT_SETTINGS, &m_dvMode );

	DisplayDevice[0].cb = sizeof ( DISPLAY_DEVICE );
	DisplayDevice[1].cb = sizeof ( DISPLAY_DEVICE );

	//EnumDisplayDevices( NULL, 0, &DisplayDevice[0], 0 );
	//EnumDisplayDevices( NULL, 1, &DisplayDevice[1], 0 );

    devmode.dmSize			=sizeof(devmode);
    devmode.dmPelsWidth		= Width;
    devmode.dmPelsHeight	= Height;
    devmode.dmBitsPerPel	= 16;
    devmode.dmFields		= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	m_hFont = CreateFont ( 12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "굴림체\0" );
    memcpy ( (void *)(devmode.dmDeviceName), (void *)(DisplayDevice[0].DeviceName), 32);
	SelectObject(hMemDC, m_hFont);
	//if ( Full == TRUE ) ChangeDisplaySettings ( &devmode, CDS_FULLSCREEN );

 	ResetFrameSkip ();

	m_bInitial = TRUE;
	return TRUE;
}

void _CDIB::Release( void )
{
	if ( m_bInitial == FALSE ) return;

	SelectObject(hMemDC, holdScreen);
	DeleteObject(holdScreen);
	DeleteObject(hBMPScreen);
	DeleteDC(hMemDC);
	ReleaseDC(m_hWnd, hScreenDC);
	m_bInitial = FALSE;
}

void _CDIB::ClearScreen()
{
	strInfos.clear();
	ZeroMemory ( lpScreen, m_nWidth * m_nHeight * 3 );
	if ( !m_pZBuffer )	return;
	ZeroMemory ( m_pZBuffer, sizeof(double) * _3DviewportH * _3DviewportW );
}

DWORD _CDIB::GetPixel ( int x, int y )
{
	if ( x > m_nWidth - 1 ) return 0UL;
	if ( x < 0 ) return 0UL;
	if ( y > m_nHeight - 1 ) return 0UL;
	if ( y < 0 ) return 0UL;

	int offset = x * 3 + y * scanline;

	BYTE r = lpScreen [ offset + 0 ];
	BYTE g = lpScreen [ offset + 1 ];
	BYTE b = lpScreen [ offset + 2 ];

	return RGB(r, g, b);
}

void _CDIB::SetPixel(int x, int y, BYTE r, BYTE g, BYTE b )
{
	if ( x > m_nWidth - 1 ) return;
	if ( x < 0 ) return;
	if ( y > m_nHeight - 1 ) return;
	if ( y < 0 ) return;

	int offset = x * 3 + y * (scanline);

	lpScreen [ offset + 0 ] = b;
	lpScreen [ offset + 1 ] = g;
	lpScreen [ offset + 2 ] = r;
}

void _CDIB::SetPixel(int x, int y, DWORD color )
{
/*	if ( x > m_nWidth - 1 ) return;
	else if ( x < 0 ) return;
	if ( y > m_nHeight - 1 ) return;
	else if ( y < 0 ) return;

	int offset = x * 3 + y * scanline;

	memcpy ( (void *)(&lpScreen[offset]), (const void *)(&color), 3 );*/
	::SetPixel ( GetBackDC(), x, y, color );
}

void _CDIB::Flip( int x, int y, int w, int h )
{
	RECT	Rect;

	GetClientRect ( m_hWnd, &Rect );
	if ( w == 0 ) w = Rect.right;
	if ( h == 0 ) h = Rect.bottom;
	StretchBlt ( hScreenDC, x, y, w, h, hMemDC, 0, 0, m_nWidth, m_nHeight, SRCCOPY);
	//BitBlt(hScreenDC, 0, 0, m_nWidth, m_nHeight, hMemDC, 0, 0, SRCCOPY);
}

void _CDIB::TextOut(int x, int y, LPSTR Text)
{
	int	tab = 40;

	HFONT	hPrev = (HFONT)SelectObject ( hMemDC, m_hFont );
	::TabbedTextOut ( hMemDC, x, y, Text, strlen(Text), 1, &tab, 0);
	SelectObject ( hMemDC, hPrev );
	DeleteObject ( hPrev );
}

void _CDIB::DrawText( RECT rect, LPSTR Text, unsigned int align )
{
	HFONT	hPrev = (HFONT)SelectObject ( hMemDC, m_hFont );
	::DrawTextA ( hMemDC, Text, strlen(Text), &rect, align );
	SelectObject ( hMemDC, hPrev );
	DeleteObject ( hPrev );
}

void _CDIB::DrawText (int x, int y, int w, int h, LPSTR Text)
{
	HFONT	hPrev = (HFONT)SelectObject ( hMemDC, m_hFont );
	RECT	rect;
	SetRect ( &rect, x, y, x + w, h + y );
	::DrawTextA ( hMemDC, Text, strlen(Text), &rect, 0 );
	SelectObject ( hMemDC, hPrev );
	DeleteObject ( hPrev );
}

LPBYTE _CDIB::GetBufferPoint()
{
	return lpScreen;
}

BOOL		_CDIB::FrameSkip ( float FramePerSec )
{
	return TRUE;
}

void		_CDIB::ResetFrameSkip ( void )
{
}

void		_CDIB::RenderCross	( int x, int y, int w, int h, int nThick, DWORD StrokeColor )
{
	RenderLine ( x-w, y, x+w, y, 0, nThick, StrokeColor );
	RenderLine ( x, y-h, x, y+h, 0, nThick, StrokeColor );
}

VectorX3	light(1, 1, 1);
void		Swap	( VectorX3* a, VectorX3* b )
{
	VectorX3	t = *a;
	*a = *b;
	*b = t;
}

void		_CDIB::RenderTriangle2		( VectorX3* pos, DWORD color )
{
	VectorX3	Pos[3] = {pos[0], pos[1], pos[2]}, UPos[3], DPos[3], Tmp;
	double		minY =-9000000000;
	double		maxY = 9000000000;
	int			nMax = 0, nMin = 2, nEls = 1;
	if ( Pos[0].y > Pos[1].y )	Swap( &Pos[0], &Pos[1] );
	if ( Pos[0].y > Pos[2].y )	Swap( &Pos[0], &Pos[2] );
	if ( Pos[1].y > Pos[2].y )	Swap( &Pos[1], &Pos[2] );

	VectorX3	D	= Pos[nMax] - Pos[nMin];
	VectorX3	d	= Pos[nMax] - Pos[nEls];
	double		rate= fabs((d.y)/D.y);
	if ( fabs(D.y) < 0.0000001 )
	{
		if ( Pos[0].x > Pos[1].x )	Swap( &Pos[0], &Pos[1] );
		if ( Pos[0].x > Pos[2].x )	Swap( &Pos[0], &Pos[2] );
		if ( Pos[1].x > Pos[2].x )	Swap( &Pos[1], &Pos[2] );
		RenderLine ( Pos[0], Pos[2], 0, 1, color );
		return;
	}
	Tmp		= Pos[nMax] - D * rate;
	DPos[0]	= Pos[nMax];
	DPos[1]	= (Pos[nEls].x < Tmp.x) ? Pos[nEls] : Tmp;
	DPos[2]	= (Pos[nEls].x < Tmp.x) ? Tmp : Pos[nEls];
	UPos[0]	= DPos[1];
	UPos[1]	= DPos[2];
	UPos[2]	= Pos[nMin];
	RenderTriangleDnH	( DPos, color );
	RenderTriangleUpH	( UPos, color );
}

void		_CDIB::RenderTriangleUpH	( VectorX3* pos, DWORD color )
{
	int		y0 = __max(0, __min(_3DviewportH-1, pos[0].y));
	int		y1 = __max(0, __min(_3DviewportH-1, pos[2].y));
	double	a0 = (pos[0].y - pos[2].y) / (pos[0].x - pos[2].x);
	double	a1 = (pos[1].y - pos[2].y) / (pos[1].x - pos[2].x);
	double	b0 = pos[0].y-a0*pos[0].x;
	double	b1 = pos[1].y-a1*pos[1].x;
	int	d0 = (pos[0].x - pos[2].x);
	int	d1 = (pos[1].x - pos[2].x);
	for ( int i = y1; i > y0; i -- )
	{
		int	x0 = __max(0, __min( _3DviewportW, d0==0 ? pos[0].x : (double)(i - b0) / a0));
		int	x1 = __max(0, __min( _3DviewportW, d1==0 ? pos[1].x : (double)(i - b1) / a1));
		for ( int j = x0; j <= x1; j ++ )
		{
			if ( m_pZBuffer )
			{
				double	z = m_pZBuffer[j+i*_3DviewportW];
				if ( z < pos[2].z && z > 0.00000001 )	continue;
				m_pZBuffer[j+i*_3DviewportW] = pos[2].z;
			}
			::SetPixel ( hMemDC, j, i, color );
		}
	}
}

void		_CDIB::RenderTriangleDnH	( VectorX3* pos, DWORD color )
{
	int		y0 = __max(0, __min(_3DviewportH-1, pos[0].y+0.5));
	int		y1 = __max(0, __min(_3DviewportH-1, pos[1].y));
	double	a0 = (pos[1].y - pos[0].y) / (pos[1].x - pos[0].x);
	double	a1 = (pos[2].y - pos[0].y) / (pos[2].x - pos[0].x);
	double	b0 = pos[0].y-a0*pos[0].x;
	double	b1 = pos[0].y-a1*pos[0].x;
	int	d0 = (pos[1].x - pos[0].x);
	int	d1 = (pos[2].x - pos[0].x);
	for ( int i = y0; i <= y1; i ++ )
	{
		int	x0 = __max(0, __min( _3DviewportW, d0==0 ? pos[1].x : (double)(i - b0) / a0));
		int	x1 = __max(0, __min( _3DviewportW, d1==0 ? pos[2].x : (double)(i - b1) / a1));
		for ( int j = x0; j <= x1; j ++ )
		{
			if ( m_pZBuffer )
			{
				double	z = m_pZBuffer[j+i*_3DviewportW];
				if ( z < pos[2].z && z > 0.00000001 )	continue;
				m_pZBuffer[j+i*_3DviewportW] = pos[0].z;
			}
			::SetPixel ( hMemDC, j, i, color );
		}
	}
}

void		_CDIB::RenderElllipse	( int x, int y, int minR, int maxR, float Rotate, unsigned long color )
{
//	Rotate *= (3.14159265358979f / 180.0f);
	Matrix4x4	rot;
	rot.RotationZ(Rotate);
	for ( float	a = 0; a < 360; a += 5 )
	{
		float px = cos((a*3.14159265358979323f)/180) * maxR;
		float py =-sin((a*3.14159265358979323f)/180) * minR;
		VectorX3	src(px, py, 0), rotated;
		VectorX3Transform ( &rotated, &src, &rot );
		::SetPixel ( hMemDC, rotated.x+x, rotated.y+y, color );
	}
}

void		_CDIB::RenderTriangle	( VectorX3* pos, DWORD color, bool bLighting )
{
	Matrix4x4	m = m_mView * m_mProj;
	Vector4		posW[3];
	VectorX3	posZ[3], a, b, c;
	posW[0] = ::WorldToScreenW ( pos+0, &m, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	posW[1] = ::WorldToScreenW ( pos+1, &m, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	posW[2] = ::WorldToScreenW ( pos+2, &m, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	posZ[0] = posW[0];
	posZ[1] = posW[1];
	posZ[2] = posW[2];
	a = posZ[0] - posZ[1];
	b = posZ[1] - posZ[2];
	c = VectorX3Cross ( &a, &b );
	if ( c.z < 0 )	return;
	if ( posW[0].w < 0 )	return;
	if ( posW[1].w < 0 )	return;
	if ( posW[2].w < 0 )	return;

	a = pos[0] - pos[1];
	b = pos[1] - pos[2];
	c = VectorX3Cross ( &a, &b );
	c.Normalize();
	light.Normalize();
	double		d = __max(0, VectorX3Dot ( &c, &light )*0.5+0.5);
	double		R = (double)(color>>16 & 0x000000ff);
	double		G = (double)(color>>8 & 0x000000ff);
	double		B = (double)(color & 0x000000ff);
	unsigned int	mb = (unsigned int)__min(255, R*d);
	unsigned int	mg = (unsigned int)__min(255, G*d);
	unsigned int	mr = (unsigned int)__min(255, B*d);
	color = 0;
	color = mr << 16 | mg << 8 | mb;

	RenderTriangle2 ( posZ, color );
}

void		_CDIB::CreateZBuffer ( void )
{
	m_pZBuffer = new double [_3DviewportW*_3DviewportH];
}

void		_CDIB::RenderNGon		( VectorX3* pos, int nCnt, DWORD Color, double scale, bool bFill, bool bLighting )
{
	int	i = 0, pCount = 0, pt = -1;
	VectorX3	vt[50], clip;
	if ( nCnt < 3 )		return;
	for ( i = 0; i < nCnt; i ++ )
	{
		VectorX3Transform ( &vt[i], &(pos[i]*scale), &m_mView );
		if ( vt[i].z > 0 )
		{
			pCount ++;
			pt = i;
		}
	}
	if ( pCount < 1 )	return;
	if ( bLighting )
	{
		VectorX3	n = VectorX3Cross ( &(pos[0] - pos[1]), &(pos[1] - pos[2]) );
		light.Normalize();
		n.Normalize();
		if ( n.y < 0 )	n *= -1;
		double		d = fabs(VectorX3Dot ( &n, &light ));
		double		r = (double)(Color>>16 & 0x000000ff);
		double		g = (double)(Color>>8 & 0x000000ff);
		double		b = (double)(Color & 0x000000ff);
		unsigned int	mr = (unsigned int)(r*d);
		unsigned int	mg = (unsigned int)(g*d);
		unsigned int	mb = (unsigned int)(b*d);
		Color = 0;
		Color = mr << 16 | mg << 8 | mb;
	}
	if ( pCount != nCnt )
	{
		for ( i = 0; i < nCnt; i ++ )
		{
			if ( i == pt )		continue;
			if ( vt[i].z > 0 )	continue;
			clip = IntersectionCoordLineToPlane ( vt[pt], vt[i]-vt[pt], VectorX3(0, 0, 1), -1.0 );
			vt[i] = clip;
		}
	}
	LOGBRUSH	Brush;
	POINT		points[50];
	Brush.lbStyle = (!bFill)?BS_HOLLOW:BS_SOLID;
	Brush.lbColor = Color;
	for ( i = 0; i < nCnt; i ++ )
	{
		Vector2 a = GetScreenPosV ( vt[i] );
		points[i].x = a.x;
		points[i].y = a.y;
	}

	HPEN	pen = CreatePen	( 0, 1, Color);
	HPEN	old = (HPEN)SelectObject( hMemDC, pen );
	HBRUSH	brs	= CreateBrushIndirect( &Brush );
	HBRUSH	olb = (HBRUSH)SelectObject( GetBackDC(), brs );
	Polygon ( hMemDC, points, nCnt );
	SelectObject( GetBackDC(), olb );
	SelectObject( GetBackDC(), old );
	DeleteObject( brs );
	DeleteObject( pen );
}

void		_CDIB::RenderLine	( int x, int y, int x1, int y1, int nStyle, int nThick, DWORD Color )
{
	POINT	Pos2D;
	HPEN pen = CreatePen	( nStyle, nThick, Color);
	HPEN old = (HPEN)SelectObject( GetBackDC(), pen );
	MoveToEx( GetBackDC(), x, y, &Pos2D );
	LineTo( GetBackDC(), x1, y1 );
	SelectObject( GetBackDC(), old );
	DeleteObject( pen );
	return;

	int	i, j;
	if ( x1 < 0 && x < 0 )	return;
	if ( y1 < 0 && y < 0 )	return;
	int		dx	= abs(x1 - x);
	int		dy	= abs(y1 - y);
	double	a	= 0, b = 0;
	int		step = (nStyle+1);
	if		( dx == 0 )
	{
		for ( j = 0; j < nThick-1; j ++ )
		{
			for ( i = __min(y, y1); i <= __max(y, y1); i += step )
			{
				::SetPixel ( hMemDC, x+j, i, Color );
				::SetPixel ( hMemDC, x-j, i, Color );
			}
		}
		for ( i = __min(y, y1); i <= __max(y, y1); i += step )	::SetPixel ( hMemDC, x, i, Color );
	}
	else if	( dy == 0 )
	{
		for ( j = 0; j < nThick-1; j ++ )
		{
			for ( i = __min(x, x1); i <= __max(x, x1); i += step )
			{
				::SetPixel ( hMemDC, i, y+j, Color );
				::SetPixel ( hMemDC, i, y-j, Color );
			}
		}
		for ( i = __min(x, x1); i <= __max(x, x1); i += step )	::SetPixel ( hMemDC, i, y, Color );
	}
	else if	( dx > dy )
	{
		a	= (double)(y1 - y) / (double)(x1 - x);
		b	= (double)y - a * (double)x;
		x = __min(m_nWidth, __max(0, x));
		x1= __min(m_nWidth, __max(0, x1));
		y = __min(m_nHeight, __max(0, y));
		y1= __min(m_nHeight, __max(0, y1));
		for ( i = x; ((x1>x)?i<=x1:i>=x1); ((x1>x)?i+=step:i-=step) )
		{
			j = i * a + b;
			if ( j < 0 )	continue;
			if ( i < 0 )	continue;
			::SetPixel ( hMemDC, i, j, Color );
		}
	}
	else
	{
		a	= (double)(x1 - x) / (double)(y1 - y);
		b	= (double)x - a * (double)y;
		x = __min(m_nWidth, __max(0, x));
		x1= __min(m_nWidth, __max(0, x1));
		y = __min(m_nHeight, __max(0, y));
		y1= __min(m_nHeight, __max(0, y1));
		for ( i = y; ((y1>y)?i<=y1:i>=y1); ((y1>y)?i+=step:i-=step) )
		{
			j = i * a + b;
			if ( j < 0 )	continue;
			if ( i < 0 )	continue;
			::SetPixel ( hMemDC, j, i, Color );
		}
	}
}

void	_CDIB::RenderCircle( int x, int y, unsigned int r, int nStyle, int nThick, DWORD Color )
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

void	_CDIB::RenderRect		( RECT rect, int nThick, DWORD StrokeColor, DWORD SolidColor, bool bFill )
{
	int	x = rect.left;
	int y = rect.top;
	int w = rect.right-rect.left+1;
	int h = rect.bottom-rect.top+1;
	RenderBox	( x, y, w, h, nThick, StrokeColor, SolidColor, bFill );
}

void	_CDIB::drawGradientRect	( int x, int y, int w, int h, unsigned long color0, unsigned long color1, unsigned long mode )
{
	RECT	r;
	SetRect ( &r, x, y, x+w-1, y+h-1 );
	TRIVERTEX	vertex[2];
	vertex[0].x     = r.left;
	vertex[0].y     = r.top;
	vertex[0].Red   = color0<<24>>24<<8;
	vertex[0].Green = color0<<16>>24<<8;
	vertex[0].Blue  = color0<< 8>>24<<8;
	vertex[0].Alpha = 0x0000;

	vertex[1].x     = r.right;
	vertex[1].y     = r.bottom; 
	vertex[1].Red   = color1<<24>>24<<8;
	vertex[1].Green = color1<<16>>24<<8;
	vertex[1].Blue  = color1<< 8>>24<<8;
	vertex[1].Alpha = 0x0000;
	GRADIENT_RECT gRect;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	// Draw a shaded rectangle. 
	GradientFill(GetBackDC(), vertex, 2, &gRect, 1, mode);
}

void	_CDIB::RenderBox	( int x, int y, int w, int h, int nThick, DWORD StrokeColor, DWORD SolidColor, bool bFill )
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

void	_CDIB::RenderFlipBox( VectorX3 Min, VectorX3 Max, DWORD StrokeColor )
{
	VectorX3	box[8];
	VectorX3	sorted[4];
	byte		link[24];
	int			maxZ = -1, i;
	double		fmaxZ = -10000000000000;
	VectorX3	src[8] = {
							VectorX3(Min.x, Min.y, Min.z),
							VectorX3(Max.x, Min.y, Min.z),
							VectorX3(Min.x, Max.y, Min.z),
							VectorX3(Min.x, Min.y, Max.z),
							VectorX3(Max.x, Max.y, Max.z),
							VectorX3(Min.x, Max.y, Max.z),
							VectorX3(Max.x, Min.y, Max.z),
							VectorX3(Max.x, Max.y, Min.z)
							};

	VectorX3	wld[8];
	int			pt = -1;
	memcpy ( wld, src, sizeof(VectorX3)*8 );
	
	for ( i = 0; i < 8; i ++ )
	{
		VectorX3Transform ( &wld[i], &src[i], &m_mView );
		if ( wld[i].z > 0 )	pt = i;
	}
	if ( pt == -1 )	return;
	for ( i = 0; i < 8; i ++ )
	{
		VectorX3	vt = wld[i];
		if ( pt != -1 && pt != i && wld[i].z < 0 )
		{
			VectorX3	clip = IntersectionCoordLineToPlane ( wld[i], wld[pt]-wld[i], VectorX3(0, 0, -1), 1.0 );
			vt = clip;
		}
		box[i] = WorldToScreen3 ( &(Vector4(vt.x, vt.y, vt.z, 1)), &m_mProj, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	}
	link[ 0] = 0;	link[ 1] = 3;	link[ 2] = 6;	link[ 3] = 1;
	link[ 4] = 0;	link[ 5] = 1;	link[ 6] = 7;	link[ 7] = 2;
	link[ 8] = 0;	link[ 9] = 2;	link[10] = 5;	link[11] = 3;
	link[12] = 4;	link[13] = 6;	link[14] = 3;	link[15] = 5;
	link[16] = 4;	link[17] = 5;	link[18] = 2;	link[19] = 7;
	link[20] = 4;	link[21] = 7;	link[22] = 1;	link[23] = 6;
	int	apc = 4;
	for ( i = 0; i < 24; i += 4 )
	{
		VectorX3	cross	= VectorX3Cross ( &(box[link[i+1]]-box[link[i+0]]), &(box[link[i+2]]-box[link[i+0]]));
		cross.Normalize();
		double		Dot		= VectorX3Dot ( &cross, &VectorX3(0, 0, 1) );

		if ( Dot > 0 )	continue;
		cross	= VectorX3Cross ( &(wld[link[i+1]]-wld[link[i+0]]), &(wld[link[i+2]]-wld[link[i+0]]));
		cross.Normalize();
		Dot		= VectorX3Dot ( &cross, &VectorX3(0, 0, 1) );
		int		nScreen = 0;
		for ( int k = 0; k < 4; k ++ )
		{
			sorted[k] = src[link[i+k]];
			nScreen += (sorted[k].z > 1);
		}

		float segR	= (nScreen > 3 ? 1.0f : 0.21f)*pow(__min(0.8, __max(0,Dot*0.5+0.5)), 2)*255.0f;
		float segG	= (nScreen > 3 ? 1.0f : 0.12f)*pow(__min(0.8, __max(0,Dot*0.5+0.5)), 2)*255.0f;
		float segB	= (nScreen > 3 ? 1.0f : 0.12f)*pow(__min(0.8, __max(0,Dot*0.5+0.5)), 2)*255.0f;
		unsigned long colorR	= (segR);
		unsigned long colorG	= (segG);
		unsigned long colorB	= (segB);
		unsigned long color		= colorR | (colorG<<8) | (colorB<<16);

		RenderNGon ( sorted, apc, color, 1, true, false );
	}
}

void	_CDIB::RenderWireBox( VectorX3 Min, VectorX3 Max, DWORD StrokeColor )
{
	VectorX3	box[8];
	VectorX3	sorted[24];
	byte		link[24];
	int			maxZ = -1, i;
	double		fmaxZ = -10000000000000;
	VectorX3	src[8] = {
							VectorX3(Min.x, Min.y, Min.z),
							VectorX3(Max.x, Min.y, Min.z),
							VectorX3(Min.x, Max.y, Min.z),
							VectorX3(Min.x, Min.y, Max.z),
							VectorX3(Max.x, Max.y, Max.z),
							VectorX3(Min.x, Max.y, Max.z),
							VectorX3(Max.x, Min.y, Max.z),
							VectorX3(Max.x, Max.y, Min.z)
							};

	VectorX3	wld[8];
	int			pt = -1;
	memcpy ( wld, src, sizeof(VectorX3)*8 );
	
	for ( i = 0; i < 8; i ++ )
	{
		VectorX3Transform ( &wld[i], &src[i], &m_mView );
		if ( wld[i].z > 0 )	pt = i;
	}
	for ( i = 0; i < 8; i ++ )
	{
		VectorX3	vt = wld[i];
		if ( pt != -1 && pt != i && wld[i].z < 0 )
		{
			VectorX3	clip = IntersectionCoordLineToPlane ( wld[i], wld[pt]-wld[i], VectorX3(0, 0, -1), 1.0 );
			vt = clip;
		}
		box[i] = WorldToScreen3 ( &(Vector4(vt.x, vt.y, vt.z, 1)), &m_mProj, _3DviewportX, _3DviewportY, 1, 1 );
	}
	link[ 0] = 0;	link[ 1] = 3;	link[ 2] = 6;	link[ 3] = 1;
	link[ 4] = 0;	link[ 5] = 1;	link[ 6] = 7;	link[ 7] = 2;
	link[ 8] = 0;	link[ 9] = 2;	link[10] = 5;	link[11] = 3;
	link[12] = 4;	link[13] = 6;	link[14] = 3;	link[15] = 5;
	link[16] = 4;	link[17] = 5;	link[18] = 2;	link[19] = 7;
	link[20] = 4;	link[21] = 7;	link[22] = 1;	link[23] = 6;
	int	apc = 4;
	for ( i = 0; i < 24; i += 4 )
	{
		VectorX3	cross	= VectorX3Cross ( &(box[link[i+1]]-box[link[i+0]]), &(box[link[i+2]]-box[link[i+0]]));
		double		Dot		= VectorX3Dot ( &cross, &VectorX3(0,0,1) );
		if ( Dot < 0 )	continue;
		for ( int k = 0; k < 4; k ++ )	sorted[k] = src[link[i+k]];
		for ( int k = 0; k < 4; k ++ )	RenderLine ( sorted[k], sorted[(k+1)%4], 2, 3, 0);
		for ( int k = 0; k < 4; k ++ )	RenderLine ( sorted[k], sorted[(k+1)%4], 2, 1, StrokeColor);
	}
}

void	_CDIB::RenderAABB	( VectorX3 Min, VectorX3 Max, DWORD StrokeColor )
{
	VectorX3	box[8];

	RenderLine ( VectorX3(Min.x, Min.y, Min.z), VectorX3(Min.x, Max.y, Min.z), 0, 3, 0 );
	RenderLine ( VectorX3(Max.x, Min.y, Min.z), VectorX3(Max.x, Max.y, Min.z), 0, 3, 0 );
	RenderLine ( VectorX3(Min.x, Min.y, Max.z), VectorX3(Min.x, Max.y, Max.z), 0, 3, 0 );
	RenderLine ( VectorX3(Max.x, Min.y, Max.z), VectorX3(Max.x, Max.y, Max.z), 0, 3, 0 );

	RenderLine ( VectorX3(Min.x, Min.y, Min.z), VectorX3(Max.x, Min.y, Min.z), 0, 3, 0 );
	RenderLine ( VectorX3(Max.x, Min.y, Min.z), VectorX3(Max.x, Min.y, Max.z), 0, 3, 0 );
	RenderLine ( VectorX3(Max.x, Min.y, Max.z), VectorX3(Min.x, Min.y, Max.z), 0, 3, 0 );
	RenderLine ( VectorX3(Min.x, Min.y, Max.z), VectorX3(Min.x, Min.y, Min.z), 0, 3, 0 );

	RenderLine ( VectorX3(Min.x, Max.y, Min.z), VectorX3(Max.x, Max.y, Min.z), 0, 3, 0 );
	RenderLine ( VectorX3(Max.x, Max.y, Min.z), VectorX3(Max.x, Max.y, Max.z), 0, 3, 0 );
	RenderLine ( VectorX3(Max.x, Max.y, Max.z), VectorX3(Min.x, Max.y, Max.z), 0, 3, 0 );
	RenderLine ( VectorX3(Min.x, Max.y, Max.z), VectorX3(Min.x, Max.y, Min.z), 0, 3, 0 );

	RenderLine ( VectorX3(Min.x, Min.y, Min.z), VectorX3(Min.x, Max.y, Min.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Max.x, Min.y, Min.z), VectorX3(Max.x, Max.y, Min.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Min.x, Min.y, Max.z), VectorX3(Min.x, Max.y, Max.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Max.x, Min.y, Max.z), VectorX3(Max.x, Max.y, Max.z), 0, 1, StrokeColor );

	RenderLine ( VectorX3(Min.x, Min.y, Min.z), VectorX3(Max.x, Min.y, Min.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Max.x, Min.y, Min.z), VectorX3(Max.x, Min.y, Max.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Max.x, Min.y, Max.z), VectorX3(Min.x, Min.y, Max.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Min.x, Min.y, Max.z), VectorX3(Min.x, Min.y, Min.z), 0, 1, StrokeColor );

	RenderLine ( VectorX3(Min.x, Max.y, Min.z), VectorX3(Max.x, Max.y, Min.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Max.x, Max.y, Min.z), VectorX3(Max.x, Max.y, Max.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Max.x, Max.y, Max.z), VectorX3(Min.x, Max.y, Max.z), 0, 1, StrokeColor );
	RenderLine ( VectorX3(Min.x, Max.y, Max.z), VectorX3(Min.x, Max.y, Min.z), 0, 1, StrokeColor );
}

void		_CDIB::RenderCircle( VectorX3 vPos, int r, int nStyle, int nThick, DWORD Color )
{
	VectorX3	tPos;
	VectorX3Transform ( &tPos, &vPos, &m_mView );
	if ( tPos.z < 0 )	return;
	Vector2	pos = WorldToScreen (&tPos, &m_mProj, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	RenderCircle( pos.x, pos.y, r, nStyle, nThick, Color );
}

void		_CDIB::RenderCross	( VectorX3 vPos, int w, int h, int nThick, DWORD StrokeColor )
{
	VectorX3	tPos;
	VectorX3Transform ( &tPos, &vPos, &m_mView );
	if ( tPos.z < 0 )	return;
	Vector2	pos = WorldToScreen (&tPos, &m_mProj, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	RenderCross ( pos.x, pos.y, w, h, nThick, StrokeColor );
}

VectorX3	_CDIB::RenderArc	( VectorX3 o, VectorX3 s, VectorX3 t, int nStyle, int nThick, DWORD Color, char* format, double k, unsigned long captionPos )
{
	va_list arg;
	SIZE	sizeT;
	CHAR szString[MAX_PATH];

	Matrix4x4	mat;
	double		size = __max(s.Length(), t.Length() );
	s.Normalize();
	t.Normalize();
	VectorX3	arc, arcPast = s, last, axis = VectorX3Cross( &s, &t );
	axis.Normalize();
	axis.x = fabs(axis.x);
	axis.y = fabs(axis.y);
	axis.z = fabs(axis.z);
	double		fAngle = acos( VectorX3Dot ( &s, &t ) );
	mat.RotationAxis ( axis, fAngle );
	VectorX3Transform ( &last, &s, &mat );
	VectorX3	cross0 = VectorX3Cross ( &s, &t );
	VectorX3	cross1 = VectorX3Cross ( &s, &last );
	double		l = VectorX3Dot ( &cross0, &cross1 );

	sprintf ( szString, "%s%5.2f\0", format, k * (l < 0 ? 1 : -1) );
	sprintf ( szString, "%s%5.2f\0", format, k );
	GetTextExtentPoint ( GetBackDC(), szString, strlen(szString), &sizeT );
	if ( fAngle * 57.2957795 > 2 )
	{
		for ( double	a = 0; a < fAngle; a += 0.0174533 )
		{
			mat.RotationAxis ( axis, a * (l < 0 ? -1 : 1) );
			VectorX3Transform ( &arc, &s, &mat );
			RenderLine ( arc * size + o, arcPast * size + o, 0, 1, 0x000000af );
			arcPast = arc;
		}
	}
	else
	{
		arcPast = s;
		for ( double	a = 0; a > -0.1745329; a -= 0.0174533 )
		{
			mat.RotationAxis ( axis, a * (l < 0 ? -1 : 1) );
			VectorX3Transform ( &arc, &s, &mat );
			RenderLine ( arc * size + o, arcPast * size + o, 0, 1, 0x000000af );
			arcPast = arc;
		}
		arcPast = t;
		for ( double	a = fAngle; a < fAngle + 0.1745329; a += 0.0174533 )
		{
			mat.RotationAxis ( axis, a * (l < 0 ? -1 : 1) );
			VectorX3Transform ( &arc, &s, &mat );
			RenderLine ( arc * size + o, arcPast * size + o, 0, 1, 0x000000af );
			arcPast = arc;
		}
	}
	VectorX3	r = (s+t)*0.5*size + o;
	if ( captionPos == 1 )
	{
		strInfos.push_back ( szString );
		int	nCount = strInfos.size();
		Matrix4x4	m = m_mView * m_mProj;
		Vector2		a1 = GetScreenPos ( r );
		Vector2		a2 = Vector2(sizeT.cx, nCount * 15 + sizeT.cy/2);
		if ( fabs(a1.x - a2.x) > abs(a1.y - a2.y) )
		{
			RenderLine ( a1.x, a1.y, a1.x - fabs(a2.y - a1.y), a2.y, 2, 1, 0x00afafff );
			RenderLine ( a1.x - fabs(a2.y - a1.y), a2.y, a2.x, a2.y, 2, 1, 0x00afafff );
		}
		else
		{
			RenderLine ( a1.x, a1.y, a2.x, a1.y - fabs(a2.x - a1.x), 2, 1, 0x00afafff );
			RenderLine ( a2.x, a1.y - fabs(a2.x - a1.x), a2.x, a2.y, 2, 1, 0x00afafff );
		}
		SetTextColor ( GetBackDC(), 0x00000000 );
		TextOutA (-1, nCount*15-1, szString );
		TextOutA ( 1, nCount*15-1, szString );
		TextOutA (-1, nCount*15+1, szString );
		TextOutA ( 1, nCount*15+1, szString );
		SetTextColor ( GetBackDC(), 0x00ffffff );
		TextOutA ( 0, nCount*15, szString );
		return r;
	}
	SetTextColor ( GetBackDC(), 0x00000000 );
	Vector2		arc2d= GetScreenPos((s+t)*0.5*size + o);
	TextOutA ( arc2d.x - sizeT.cx/2-1, arc2d.y - sizeT.cy/2-1, szString );
	TextOutA ( arc2d.x - sizeT.cx/2+1, arc2d.y - sizeT.cy/2-1, szString );
	TextOutA ( arc2d.x - sizeT.cx/2-1, arc2d.y - sizeT.cy/2+1, szString );
	TextOutA ( arc2d.x - sizeT.cx/2+1, arc2d.y - sizeT.cy/2+1, szString );
	SetTextColor ( GetBackDC(), 0x00ffffff );
	TextOutA ( arc2d.x - sizeT.cx/2+0, arc2d.y - sizeT.cy/2+0, szString );
	return r;
}

void		_CDIB::RenderArrow		( VectorX3 vPos, VectorX3 D, VectorX3 N, int nStyle, int nThick, DWORD Color, char* szCaption )
{
	VectorX3	Cross = VectorX3Cross ( &D, &N );
	VectorX3	Wing[2] = {Cross + D*2, -Cross + D*2};
	Wing[0].Normalize();
	Wing[1].Normalize();
	RenderLine ( vPos, vPos + D, nStyle, nThick, Color );
	RenderLine ( vPos, vPos + Wing[0]*10, nStyle, nThick, Color );
	RenderLine ( vPos, vPos + Wing[1]*10, nStyle, nThick, Color );
}

void		_CDIB::RenderLine	( VectorX3 vMin, VectorX3 vMax, int nStyle, int nThick, DWORD Color )
{
	VectorX3	clip = VectorX3(0,0,0);
	VectorX3	tMax, tMin;
	VectorX3Transform ( &tMax, &vMax, &m_mView );
	VectorX3Transform ( &tMin, &vMin, &m_mView );
	if	( tMax.z < 0 && tMin.z < 0 )	return;
/*	if	( tMin.z < 0 )
	{
		clip = IntersectionCoordLineToPlane ( tMin, tMax-tMin, VectorX3(0, 0, -1), 1.0 );
		tMin = clip;
	}
	if	( tMax.z < 0 )
	{
		clip = IntersectionCoordLineToPlane ( tMax, tMin-tMax, VectorX3(0, 0, -1), 1.0 );
		tMax = clip;
	}*/
	Vector2	pos0 = WorldToScreen (&tMin, &m_mProj, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	Vector2	pos1 = WorldToScreen (&tMax, &m_mProj, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
	RenderLine ( pos0.x, pos0.y, pos1.x, pos1.y, nStyle, nThick, Color );
}

Vector2		_CDIB::GetScreenPos	( VectorX3 a )
{
	return WorldToScreen (&a, &m_mVP, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
}

Vector2		_CDIB::GetScreenPosV	( VectorX3 a )
{
	return WorldToScreen (&a, &m_mProj, _3DviewportX, _3DviewportY, _3DviewportW, _3DviewportH );
}

void	_CDIB::RenderGraph	( int x, int y, int w, int t, int border, int solid, double data )
{
	RenderBox ( x, y-data, w, data, t, border, solid, true );
}
