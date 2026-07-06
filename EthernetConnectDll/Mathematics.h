#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4996 )
#pragma once
#include <windows.h>
#include <math.h>

#define SWAP32(l) ( ((((l) & 0xff000000) >> 24) | (((l) & 0x00ff0000) >> 8)  | (((l) & 0x0000ff00) << 8)  | (((l) & 0x000000ff) << 24)))
#define	FRAME_COUNT		(100)
#define	_clip(a,b,c)	(__min((b),__max((a),(c))))

#define	UNDERSAMPLE_STEP		(4)

//#define		SIZEW		(672)
//#define		SIZEH		(512)
#define		SIZEW		(352)
#define		SIZEH		(420)
#define		STRIDE4		(SIZEW/4)
#define		UNDERW		(((4-(STRIDE4%4))%4)+STRIDE4)

#define		MODE_ALIGNED_PY	(0)	/*업벡터가 위를 보는 랜드스케이프 모드*/
#define		MODE_ALIGNED_NY	(1)	/*업벡터가 아래를 보는 랜드스케이프 모드*/
#define		MODE_ALIGNED_NX	(2)	/*업벡터가 -X를 보는 포트레이트 모드*/
#define		MODE_ALIGNED_PX	(3)	/*업벡터가 +X를 보는 포트레이트 모드*/

#define	TORAD		(0.0174532925199433)
#define	TODEG		(57.29577951308233)
#pragma intrinsic	(sin, cos, sqrt)
#ifndef fsin
__inline	double	fsin(double input)
{
	return sin(input);
}
#endif
#ifndef fcos
__inline	double	fcos(double input)
{
	return cos(input);
}
#endif
#ifndef ftan
__inline	double	ftan(double input)
{
	return tan(input);
}
#endif
#ifndef fsqr
__inline	double	fsqr(double input)
{
	return sqrt(input);
}
#endif

__inline	int	fsign(double x)
{
	if(x > 0.f) 
		return 1;
	else 
		return -1;
}

__inline
float clamp(float x, float lowerlimit, float upperlimit) {
  if (x < lowerlimit)
    x = lowerlimit;
  if (x > upperlimit)
    x = upperlimit;
  return x;
}

__inline
float smootherstep(float edge0, float edge1, float x) {
  // Scale, and clamp x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  // Evaluate polynomial
  return x * x * x * (x * (x * 6 - 15) + 10);
}

static	double			dfBasisTime;
__inline
double			fTimeGetTime				( void )
{
	LARGE_INTEGER	Count, Freq;
	QueryPerformanceCounter		( &Count );
	QueryPerformanceFrequency	( &Freq );
	double			fCount	= (double)Count.QuadPart;
	double			fFreq	= (double)(Freq.QuadPart);
	return (fCount/fFreq*1000) - dfBasisTime;
}

struct	RECTANGLE
{
	short	l, t, r, b;
	__inline	int		w		(void){return r-l;}
	__inline	int		h		(void){return b-t;}
	__inline	double	aspect	(void)
	{
		if ( w() == 0 )	return 0;
		if ( h() == 0 )	return 0;
		return (double)(__min(w(), h()))/(double)(__max(w(), h()));
	}
	__inline	double	aspect_abs	(void)
	{
		if ( w() == 0 )	return 0;
		if ( h() == 0 )	return 0;
		return (double)(w()/h());
	}
	__inline	bool	isInclude	( int x, int y )
	{
		if ( l > x && l > -6000 )	return false;
		if ( r < x && r > -6000 )	return false;
		if ( t > y && t > -6000 )	return false;
		if ( b < y && b > -6000 )	return false;
		return true;
	}
	__inline	__int64*	INTP( void )
	{
		return (__int64*)(&l);
	}
};

__inline	void	SetRectangle	( RECTANGLE* r, int a, int b, int c, int d, bool bZero )
{
	r->l = a;
	r->t = b;
	r->r = c;
	r->b = d;
	if ( !bZero )	return;
	r->l = __max( 0, a);
	r->t = __max( 0, b);
	r->r = __max( 0, c);
	r->b = __max( 0, d);
}
struct VectorX3;

struct Vector2
{
	double	x, y;
public:
    Vector2() {};
    Vector2( double x, double y );

    operator double* ();
    // assignment operators
    Vector2& operator += ( CONST Vector2& );
    Vector2& operator -= ( CONST Vector2& );
	Vector2& operator *= ( const Vector2& );
	Vector2& operator /= ( const Vector2& );
    Vector2& operator *= ( double );
    Vector2& operator /= ( double );

    // unary operators
    Vector2 operator + () const;
    Vector2 operator - () const;
	Vector2 operator + (double) const;
	Vector2 operator - (double) const;

    // binary operators
    Vector2 operator + ( CONST Vector2& ) const;
    Vector2 operator - ( CONST Vector2& ) const;
	Vector2 operator * ( const Vector2& ) const;
	Vector2 operator / ( const Vector2& ) const;
    Vector2 operator * ( double ) const;
    Vector2 operator / ( double ) const;

    friend Vector2 operator * ( double, CONST struct Vector2& );

    BOOL operator == ( CONST Vector2& ) const;
    BOOL operator != ( CONST Vector2& ) const;
    void	Normalize	( void );
    double	Length		( void );
	__inline	Vector2	yx	(void)	{return Vector2(y, x);}
	__inline	Vector2	clip(int x0, int y0, int x1, int y1)
	{
		return Vector2(__max(x0, __min(x1, x)), __max(y0, __min(y1, y)));
	}
	VectorX3	unprojectOrthoSphere(double targetRadius=1.0, bool bBackFace=false);
};
double		Vector2Dot(Vector2* a, const Vector2* b );

struct VectorX3
{
	double	x, y, z;
public:
    VectorX3() {};
    VectorX3( double x, double y, double z );

    operator double* ();
    operator Vector2 ();

    // assignment operators
    VectorX3& operator += ( CONST VectorX3& );
    VectorX3& operator -= ( CONST VectorX3& );
    VectorX3& operator *= ( double );
    VectorX3& operator /= ( double );

    // unary operators
    VectorX3 operator + () const;
    VectorX3 operator - () const;

    // binary operators
    VectorX3 operator + ( CONST VectorX3& ) const;
    VectorX3 operator - ( CONST VectorX3& ) const;
    VectorX3 operator * ( double ) const;
    VectorX3 operator / ( double ) const;

    friend VectorX3 operator * ( double, CONST struct VectorX3& );

    BOOL operator == ( CONST VectorX3& ) const;
    BOOL operator != ( CONST VectorX3& ) const;
    void	Normalize	( void );
    double	Length		( void );
};
double		VectorX3Dot(VectorX3* a, const VectorX3* b );
VectorX3	VectorX3Cross(VectorX3* a, const VectorX3* b );

struct	VectorS3
{
	short	x, y, z;
    __inline	VectorS3() {x = 0;y = 0;z = 0;};
    __inline	VectorS3(VectorX3 a) {x = a.x;y = a.y;z = a.z;}
};

struct Vector4 : public VectorX3
{
	double	w;
	Vector4(){};
	Vector4( double x, double y, double z, double w);
};

__inline
Vector4::Vector4( double fx, double fy, double fz, double fw)
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}

__inline
VectorX3::VectorX3( double fx, double fy, double fz )
{
    x = fx;
    y = fy;
    z = fz;
}


__inline
VectorX3::operator double* ()
{
    return (double *) &x;
}

__inline
VectorX3::operator Vector2 ()
{
    return Vector2(x, y);
}

// assignment operators
__inline VectorX3&
VectorX3::operator += ( CONST VectorX3& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

__inline VectorX3&
VectorX3::operator -= ( CONST VectorX3& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

__inline VectorX3&
VectorX3::operator *= ( double f )
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

__inline VectorX3&
VectorX3::operator /= ( double f )
{
    double fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    return *this;
}


// unary operators
__inline VectorX3
VectorX3::operator + () const
{
    return *this;
}

__inline VectorX3
VectorX3::operator - () const
{
    return VectorX3(-x, -y, -z);
}


// binary operators
__inline VectorX3
VectorX3::operator + ( CONST VectorX3& v ) const
{
    return VectorX3(x + v.x, y + v.y, z + v.z);
}

__inline VectorX3
VectorX3::operator - ( CONST VectorX3& v ) const
{
    return VectorX3(x - v.x, y - v.y, z - v.z);
}

__inline VectorX3
VectorX3::operator * ( double f ) const
{
    return VectorX3(x * f, y * f, z * f);
}

__inline VectorX3
VectorX3::operator / ( double f ) const
{
    double fInv = 1.0f / f;
    return VectorX3(x * fInv, y * fInv, z * fInv);
}


__inline VectorX3
operator * ( double f, CONST struct VectorX3& v )
{
    return VectorX3(f * v.x, f * v.y, f * v.z);
}

__inline BOOL
VectorX3::operator == ( CONST VectorX3& v ) const
{
    return x == v.x && y == v.y && z == v.z;
}

__inline BOOL
VectorX3::operator != ( CONST VectorX3& v ) const
{
    return x != v.x || y != v.y || z != v.z;
}

__inline
void	VectorX3::Normalize( void )
{
	double	fLeng = sqrt(VectorX3Dot(&VectorX3(x,y,z), &VectorX3(x,y,z)));
	if ( fLeng < 0.0000000001 )	return;
	x /= fLeng;
	y /= fLeng;
	z /= fLeng;
}

__inline
double	VectorX3::Length( void )
{
	return sqrt(VectorX3Dot(&VectorX3(x,y,z), &VectorX3(x,y,z)));
}


__inline
Vector2::Vector2( double fx, double fy )
{
    x = fx;
    y = fy;
}


__inline
Vector2::operator double* ()
{
    return (double *) &x;
}
// assignment operators
__inline Vector2&
Vector2::operator += ( CONST Vector2& v )
{
    x += v.x;
    y += v.y;
    return *this;
}

__inline Vector2&
Vector2::operator -= ( CONST Vector2& v )
{
    x -= v.x;
    y -= v.y;
    return *this;
}

__inline Vector2&
Vector2::operator *= ( const Vector2& v )
{
	x *= v.x;
	y *= v.y;
	return *this;
}

__inline Vector2&
Vector2::operator /= ( const Vector2& v )
{
	x /= v.x;
	y /= v.y;
	return *this;
}
__inline Vector2&
Vector2::operator *= ( double f )
{
    x *= f;
    y *= f;
    return *this;
}

__inline Vector2&
Vector2::operator /= ( double f )
{
    double fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    return *this;
}


// unary operators
__inline Vector2
Vector2::operator + () const
{
    return *this;
}

__inline Vector2
Vector2::operator - () const
{
    return Vector2(-x, -y);
}


__inline Vector2
Vector2::operator + (double a) const
{
	return Vector2(x+a, y+a);
}

__inline Vector2
Vector2::operator - (double a) const
{
	return Vector2(x-a, y-a);
}

// binary operators
__inline Vector2
Vector2::operator + ( CONST Vector2& v ) const
{
    return Vector2(x + v.x, y + v.y);
}

__inline Vector2
Vector2::operator - ( CONST Vector2& v ) const
{
    return Vector2(x - v.x, y - v.y);
}

__inline Vector2
Vector2::operator * ( const Vector2& v ) const
{
	return Vector2(x * v.x, y * v.y);
}

__inline Vector2
Vector2::operator / ( const Vector2& v ) const
{
	return Vector2(x / v.x, y / v.y);
}

__inline Vector2
Vector2::operator * ( double f ) const
{
    return Vector2(x * f, y * f);
}

__inline Vector2
Vector2::operator / ( double f ) const
{
    double fInv = 1.0f / f;
    return Vector2(x * fInv, y * fInv);
}


__inline Vector2
operator * ( double f, CONST struct Vector2& v )
{
    return Vector2(f * v.x, f * v.y);
}

__inline BOOL
Vector2::operator == ( CONST Vector2& v ) const
{
    return x == v.x && y == v.y;
}

__inline BOOL
Vector2::operator != ( CONST Vector2& v ) const
{
    return x != v.x || y != v.y;
}

__inline
void	Vector2::Normalize( void )
{
	double	fLeng = sqrt(Vector2Dot(&Vector2(x,y), &Vector2(x,y)));
	if ( fLeng < 0.00000000001 )
	{
		x = 0;
		y = 0;
		return;
	}
	x /= fLeng;
	y /= fLeng;
}

__inline
double	Vector2::Length( void )
{
	return sqrt(Vector2Dot(&Vector2(x,y), &Vector2(x,y)));
}


//===========================================================================
//
// Matrices
//
//===========================================================================
struct Matrix4x4
{
public:
    union
	{
        struct
		{
            double        _11, _12, _13, _14;
            double        _21, _22, _23, _24;
            double        _31, _32, _33, _34;
            double        _41, _42, _43, _44;

        };
        double m[4][4];
    };
    Matrix4x4();
    Matrix4x4(	double _11, double _12, double _13, double _14,
                double _21, double _22, double _23, double _24,
                double _31, double _32, double _33, double _34,
                double _41, double _42, double _43, double _44 );


    // access grants
    double& operator () ( UINT Row, UINT Col );
    double  operator () ( UINT Row, UINT Col ) const;

    // casting operators
    operator double* ();
    operator CONST double* () const;

    // assignment operators
    Matrix4x4& operator *= ( CONST Matrix4x4& );
    Matrix4x4& operator += ( CONST Matrix4x4& );
    Matrix4x4& operator -= ( CONST Matrix4x4& );
    Matrix4x4& operator *= ( double );
    Matrix4x4& operator /= ( double );

    // unary operators
    Matrix4x4 operator + () const;
    Matrix4x4 operator - () const;

    // binary operators
    Matrix4x4 operator * ( CONST Matrix4x4& ) const;
    Matrix4x4 operator + ( CONST Matrix4x4& ) const;
    Matrix4x4 operator - ( CONST Matrix4x4& ) const;
    Matrix4x4 operator * ( double ) const;
    Matrix4x4 operator / ( double ) const;

    friend Matrix4x4 operator * ( double, CONST Matrix4x4& );

    BOOL operator == ( CONST Matrix4x4& ) const;
    BOOL operator != ( CONST Matrix4x4& ) const;
	bool				View		( VectorX3 pos, VectorX3 dir, VectorX3 up );
	bool				Inverse		( void );
	bool				Transpose	( void );
	bool				Perpective	( Matrix4x4* pOut, double fov, double Aspect, double zn, double zf );
	bool				RotationX	( double a );
	bool				RotationY	( double a );
	bool				RotationZ	( double a );
	bool				RotationAxis( VectorX3 v, double a );

	__inline	void	Identity	( void )
	{
		_11 = 1; _12 = 0; _13 = 0; _14 = 0;
		_21 = 0; _22 = 1; _23 = 0; _24 = 0;
		_31 = 0; _32 = 0; _33 = 1; _34 = 0;
		_41 = 0; _42 = 0; _43 = 0; _44 = 1;
	}
};

//--------------------------
// Matrix
//--------------------------
__inline
Matrix4x4*	Matrix4x4Multiply(Matrix4x4* c, const Matrix4x4* a, const Matrix4x4* b)
{
	for ( int i = 0; i < 4; i ++ )
	{
		for ( int j = 0; j < 4; j ++ )
		{
			c->m[i][j] =a->m[i][0] * b->m[0][j] +
						a->m[i][1] * b->m[1][j] +
						a->m[i][2] * b->m[2][j] +
						a->m[i][3] * b->m[3][j];
		}
	}
	return c;
}

__inline
Matrix4x4::Matrix4x4()
{
    _11 = 1; _12 = 0; _13 = 0; _14 = 0;
    _21 = 0; _22 = 1; _23 = 0; _24 = 0;
    _31 = 0; _32 = 0; _33 = 1; _34 = 0;
    _41 = 0; _42 = 0; _43 = 0; _44 = 1;
}

__inline
Matrix4x4::Matrix4x4( double f11, double f12, double f13, double f14,
                        double f21, double f22, double f23, double f24,
                        double f31, double f32, double f33, double f34,
                        double f41, double f42, double f43, double f44 )
{
    _11 = f11; _12 = f12; _13 = f13; _14 = f14;
    _21 = f21; _22 = f22; _23 = f23; _24 = f24;
    _31 = f31; _32 = f32; _33 = f33; _34 = f34;
    _41 = f41; _42 = f42; _43 = f43; _44 = f44;
}



// access grants
__inline double&
Matrix4x4::operator () ( UINT iRow, UINT iCol )
{
    return m[iRow][iCol];
}

__inline double
Matrix4x4::operator () ( UINT iRow, UINT iCol ) const
{
    return m[iRow][iCol];
}


// casting operators
__inline
Matrix4x4::operator double* ()
{
    return (double *) &_11;
}

__inline
Matrix4x4::operator CONST double* () const
{
    return (CONST double *) &_11;
}


// assignment operators
__inline Matrix4x4&
Matrix4x4::operator *= ( CONST Matrix4x4& mat )
{
    Matrix4x4Multiply(this, this, &mat);
    return *this;
}

__inline Matrix4x4&
Matrix4x4::operator += ( CONST Matrix4x4& b )
{
    _11 += b._11; _12 += b._12; _13 += b._13; _14 += b._14;
    _21 += b._21; _22 += b._22; _23 += b._23; _24 += b._24;
    _31 += b._31; _32 += b._32; _33 += b._33; _34 += b._34;
    _41 += b._41; _42 += b._42; _43 += b._43; _44 += b._44;
    return *this;
}

__inline Matrix4x4&
Matrix4x4::operator -= ( CONST Matrix4x4& b )
{
    _11 -= b._11; _12 -= b._12; _13 -= b._13; _14 -= b._14;
    _21 -= b._21; _22 -= b._22; _23 -= b._23; _24 -= b._24;
    _31 -= b._31; _32 -= b._32; _33 -= b._33; _34 -= b._34;
    _41 -= b._41; _42 -= b._42; _43 -= b._43; _44 -= b._44;
    return *this;
}

__inline Matrix4x4&
Matrix4x4::operator *= ( double f )
{
    _11 *= f; _12 *= f; _13 *= f; _14 *= f;
    _21 *= f; _22 *= f; _23 *= f; _24 *= f;
    _31 *= f; _32 *= f; _33 *= f; _34 *= f;
    _41 *= f; _42 *= f; _43 *= f; _44 *= f;
    return *this;
}

__inline Matrix4x4&
Matrix4x4::operator /= ( double f )
{
    double fInv = 1.0f / f;
    _11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
    _21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
    _31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
    _41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
    return *this;
}


// unary operators
__inline Matrix4x4
Matrix4x4::operator + () const
{
    return *this;
}

__inline Matrix4x4
Matrix4x4::operator - () const
{
    return Matrix4x4(-_11, -_12, -_13, -_14,
                      -_21, -_22, -_23, -_24,
                      -_31, -_32, -_33, -_34,
                      -_41, -_42, -_43, -_44);
}


// binary operators
__inline Matrix4x4
Matrix4x4::operator * ( CONST Matrix4x4& mat ) const
{
    Matrix4x4 matT;
    Matrix4x4Multiply(&matT, this, &mat);
    return matT;
}

__inline Matrix4x4
Matrix4x4::operator + ( CONST Matrix4x4& b ) const
{
    return Matrix4x4(_11 + b._11, _12 + b._12, _13 + b._13, _14 + b._14,
                      _21 + b._21, _22 + b._22, _23 + b._23, _24 + b._24,
                      _31 + b._31, _32 + b._32, _33 + b._33, _34 + b._34,
                      _41 + b._41, _42 + b._42, _43 + b._43, _44 + b._44);
}

__inline Matrix4x4
Matrix4x4::operator - ( CONST Matrix4x4& b ) const
{
    return Matrix4x4(_11 - b._11, _12 - b._12, _13 - b._13, _14 - b._14,
                      _21 - b._21, _22 - b._22, _23 - b._23, _24 - b._24,
                      _31 - b._31, _32 - b._32, _33 - b._33, _34 - b._34,
                      _41 - b._41, _42 - b._42, _43 - b._43, _44 - b._44);
}

__inline Matrix4x4
Matrix4x4::operator * ( double f ) const
{
    return Matrix4x4(_11 * f, _12 * f, _13 * f, _14 * f,
                      _21 * f, _22 * f, _23 * f, _24 * f,
                      _31 * f, _32 * f, _33 * f, _34 * f,
                      _41 * f, _42 * f, _43 * f, _44 * f);
}

__inline Matrix4x4
Matrix4x4::operator / ( double f ) const
{
    double fInv = 1.0f / f;
    return Matrix4x4(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
                      _21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
                      _31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
                      _41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
}


__inline Matrix4x4
operator * ( double f, CONST Matrix4x4& b )
{
    return Matrix4x4(f * b._11, f * b._12, f * b._13, f * b._14,
                      f * b._21, f * b._22, f * b._23, f * b._24,
                      f * b._31, f * b._32, f * b._33, f * b._34,
                      f * b._41, f * b._42, f * b._43, f * b._44);
}

__inline
bool		Matrix4x4::View		( VectorX3 pos, VectorX3 dir, VectorX3 up )
{
	Matrix4x4	trans;
	Matrix4x4	rot;
	VectorX3	rht	= VectorX3Cross(  &up, &dir );
	up			= VectorX3Cross( &dir, &rht );
	rht.Normalize();
	up.Normalize();
	dir.Normalize();
	rot._11 = rht.x;	rot._12 =  up.x;	rot._13 = dir.x;
	rot._21 = rht.y;	rot._22 =  up.y;	rot._23 = dir.y;
	rot._31 = rht.z;	rot._32 =  up.z;	rot._33 = dir.z;
	rot._41 = -VectorX3Dot(&rht, &(pos));
	rot._42 = -VectorX3Dot(&up,  &(pos));
	rot._43 = -VectorX3Dot(&dir, &(pos));
	memcpy ( m, rot, 8 * 16 );
	return true;
}

__inline
bool		Matrix4x4::Transpose( void )
{
}

__inline
bool		Matrix4x4::Inverse	( void )
{
	Matrix4x4	q;

	q.m[0][0]= m[1][1]*m[2][2]*m[3][3]-m[1][1]*m[2][3]*m[3][2]-m[2][1]*m[1][2]*m[3][3]+m[2][1]*m[1][3]*m[3][2]+m[3][1]*m[1][2]*m[2][3]-m[3][1]*m[1][3]*m[2][2];
	q.m[0][1]=-m[0][1]*m[2][2]*m[3][3]+m[0][1]*m[2][3]*m[3][2]+m[2][1]*m[0][2]*m[3][3]-m[2][1]*m[0][3]*m[3][2]-m[3][1]*m[0][2]*m[2][3]+m[3][1]*m[0][3]*m[2][2];
	q.m[0][2]= m[0][1]*m[1][2]*m[3][3]-m[0][1]*m[1][3]*m[3][2]-m[1][1]*m[0][2]*m[3][3]+m[1][1]*m[0][3]*m[3][2]+m[3][1]*m[0][2]*m[1][3]-m[3][1]*m[0][3]*m[1][2];
	q.m[0][3]=-m[0][1]*m[1][2]*m[2][3]+m[0][1]*m[1][3]*m[2][2]+m[1][1]*m[0][2]*m[2][3]-m[1][1]*m[0][3]*m[2][2]-m[2][1]*m[0][2]*m[1][3]+m[2][1]*m[0][3]*m[1][2];
	q.m[1][0]=-m[1][0]*m[2][2]*m[3][3]+m[1][0]*m[2][3]*m[3][2]+m[2][0]*m[1][2]*m[3][3]-m[2][0]*m[1][3]*m[3][2]-m[3][0]*m[1][2]*m[2][3]+m[3][0]*m[1][3]*m[2][2];
	q.m[1][1]= m[0][0]*m[2][2]*m[3][3]-m[0][0]*m[2][3]*m[3][2]-m[2][0]*m[0][2]*m[3][3]+m[2][0]*m[0][3]*m[3][2]+m[3][0]*m[0][2]*m[2][3]-m[3][0]*m[0][3]*m[2][2];
	q.m[1][2]=-m[0][0]*m[1][2]*m[3][3]+m[0][0]*m[1][3]*m[3][2]+m[1][0]*m[0][2]*m[3][3]-m[1][0]*m[0][3]*m[3][2]-m[3][0]*m[0][2]*m[1][3]+m[3][0]*m[0][3]*m[1][2];
	q.m[1][3]= m[0][0]*m[1][2]*m[2][3]-m[0][0]*m[1][3]*m[2][2]-m[1][0]*m[0][2]*m[2][3]+m[1][0]*m[0][3]*m[2][2]+m[2][0]*m[0][2]*m[1][3]-m[2][0]*m[0][3]*m[1][2];
	q.m[2][0]= m[1][0]*m[2][1]*m[3][3]-m[1][0]*m[2][3]*m[3][1]-m[2][0]*m[1][1]*m[3][3]+m[2][0]*m[1][3]*m[3][1]+m[3][0]*m[1][1]*m[2][3]-m[3][0]*m[1][3]*m[2][1];
	q.m[2][1]=-m[0][0]*m[2][1]*m[3][3]+m[0][0]*m[2][3]*m[3][1]+m[2][0]*m[0][1]*m[3][3]-m[2][0]*m[0][3]*m[3][1]-m[3][0]*m[0][1]*m[2][3]+m[3][0]*m[0][3]*m[2][1];
	q.m[2][2]= m[0][0]*m[1][1]*m[3][3]-m[0][0]*m[1][3]*m[3][1]-m[1][0]*m[0][1]*m[3][3]+m[1][0]*m[0][3]*m[3][1]+m[3][0]*m[0][1]*m[1][3]-m[3][0]*m[0][3]*m[1][1];
	q.m[2][3]=-m[0][0]*m[1][1]*m[2][3]+m[0][0]*m[1][3]*m[2][1]+m[1][0]*m[0][1]*m[2][3]-m[1][0]*m[0][3]*m[2][1]-m[2][0]*m[0][1]*m[1][3]+m[2][0]*m[0][3]*m[1][1];
	q.m[3][0]=-m[1][0]*m[2][1]*m[3][2]+m[1][0]*m[2][2]*m[3][1]+m[2][0]*m[1][1]*m[3][2]-m[2][0]*m[1][2]*m[3][1]-m[3][0]*m[1][1]*m[2][2]+m[3][0]*m[1][2]*m[2][1];
	q.m[3][1]= m[0][0]*m[2][1]*m[3][2]-m[0][0]*m[2][2]*m[3][1]-m[2][0]*m[0][1]*m[3][2]+m[2][0]*m[0][2]*m[3][1]+m[3][0]*m[0][1]*m[2][2]-m[3][0]*m[0][2]*m[2][1];
	q.m[3][2]=-m[0][0]*m[1][1]*m[3][2]+m[0][0]*m[1][2]*m[3][1]+m[1][0]*m[0][1]*m[3][2]-m[1][0]*m[0][2]*m[3][1]-m[3][0]*m[0][1]*m[1][2]+m[3][0]*m[0][2]*m[1][1];
	q.m[3][3]= m[0][0]*m[1][1]*m[2][2]-m[0][0]*m[1][2]*m[2][1]-m[1][0]*m[0][1]*m[2][2]+m[1][0]*m[0][2]*m[2][1]+m[2][0]*m[0][1]*m[1][2]-m[2][0]*m[0][2]*m[1][1];

	double fDetInv = 1.0f / (m[0][0]*q.m[0][0]+m[0][1]*q.m[1][0]+m[0][2]*q.m[2][0]+m[0][3]*q.m[3][0]);
	q.m[0][0]*=fDetInv;
	q.m[0][1]*=fDetInv;
	q.m[0][2]*=fDetInv;
	q.m[0][3]*=fDetInv;
	q.m[1][0]*=fDetInv;
	q.m[1][1]*=fDetInv;
	q.m[1][2]*=fDetInv;
	q.m[1][3]*=fDetInv;
	q.m[2][0]*=fDetInv;
	q.m[2][1]*=fDetInv;
	q.m[2][2]*=fDetInv;
	q.m[2][3]*=fDetInv;
	q.m[3][0]*=fDetInv;
	q.m[3][1]*=fDetInv;
	q.m[3][2]*=fDetInv;
	q.m[3][3]*=fDetInv;

	memcpy ( m, q.m, 8*16 );
	return true;
}

__inline
bool			Matrix4x4::Perpective	( Matrix4x4* pOut, double fov, double Aspect, double zn, double zf )
{
	double	yScale = 1.0f/tan(fov*0.5f);
	double	xScale = yScale / Aspect;
	_11 = xScale;	_12 = 0;		_13 = 0;				_14 = 0;
	_21 = 0;		_22 = yScale;	_23 = 0;				_24 = 0;
	_31 = 0;		_32 = 0;		_33 = zf/(zf-zn);		_34 = 1;
	_41 = 0;		_42 = 0;		_43 = -zn*zf/(zf-zn);	_44 = 0;
	if ( pOut )	memcpy ( pOut->m, m, 8*16 );
	return true;
}

__inline
bool				Matrix4x4::RotationX	( double a )
{
	Identity ();
	double SIN = sin(a);
	double COS = cos(a);
	m[1][1] = COS;
	m[2][2] = COS;
	m[1][2] = SIN;
	m[2][1] = -SIN;
	return true;
}

__inline
bool				Matrix4x4::RotationY	( double a )
{
	Identity ();
	double SIN = sin(a);
	double COS = cos(a);
	m[0][0] = COS;
	m[2][2] = COS;
	m[0][2] = -SIN;
	m[2][0] = SIN;
	return true;
}

__inline
bool				Matrix4x4::RotationZ	( double a )
{
	Identity ();
	double SIN = sin(a);
	double COS = cos(a);
	m[0][0] = COS;
	m[1][1] = COS;
	m[0][1] = SIN;
	m[1][0] = -SIN;
	return true;
}

__inline
bool				Matrix4x4::RotationAxis	( VectorX3 n, double a )
{
	n.Normalize();
	Identity ();
	double	sinq = sin(a);
	double	cosq = cos(a);
	double	n1 = n.x;
	double	n2 = n.y;
	double	n3 = n.z;
	double	n12 = n1 * n2;
	double	n13 = n1 * n3;
	double	n22 = n2 * n2;
	double	n23 = n2 * n3;
	m[0][0]	= n12 + (1 - n12)*cosq;		m[0][1] = n12*(1 - cosq) + n3*sinq;		m[0][2] = n13*(1 - cosq) - n2*sinq;	m[0][3] = 0;
	m[1][0]	= n12*(1 - cosq) - n3*sinq;	m[1][1] = n22 + (1 - n22)*cosq;			m[1][2] = n23*(1 - cosq) + n1*sinq;	m[1][3] = 0;
	m[2][0]	= n13*(1 - cosq) + n2*sinq;	m[2][1] = n23*(1 - cosq) - n1*sinq;		m[2][2] = n23 + (1 - n23)*cosq;		m[2][3] = 0;
	m[3][0]	= 0;						m[3][1] = 0;							m[3][2] = 0;						m[3][3] = 1;
	return true;
}

__inline
double		VectorX3Dot(VectorX3* a, const VectorX3* b )
{
	return (a->x * b->x)+(a->y * b->y)+(a->z * b->z);
}

__inline
double		Vector2Dot(Vector2* a, const Vector2* b )
{
	return (a->x * b->x)+(a->y * b->y);
}

__inline
VectorX3		VectorX3Cross(VectorX3* a, const VectorX3* b )
{
	return VectorX3(a->y * b->z - a->z*b->y, a->z * b->x - a->x*b->z, a->x * b->y - a->y*b->x);
}

__inline
VectorX3		IntersectionCoord2Lines	( VectorX3 P1, VectorX3 P2, VectorX3 D1, VectorX3 D2 )
{
	D1.Normalize();
	D2.Normalize();
/*	VectorX3	P21 = P2 - P1;
	double	DotD12 = VectorX3Dot(&D1, &D2);
	double	s = (VectorX3Dot( &P21, &D1 ) - (VectorX3Dot(&P21, &D2)) * DotD12) / (1.0f-DotD12*DotD12);
	double	t = (s * DotD12 - VectorX3Dot(&P21, &D2))/VectorX3Dot(&D2, &D2);
	VectorX3	Pos1 = D1 * s + P1;
	VectorX3	Pos2 = D2 * t + P2;
	return (Pos1 + Pos2)*0.5f;
*/
	VectorX3	D3		= P2-P1;
	VectorX3	cross1	= VectorX3Cross( &D3, &D2);
	VectorX3	cross2	= VectorX3Cross( &D1, &D2);
	VectorX3	cross3	= VectorX3Cross( &D3, &D1);
	double		s		= VectorX3Dot ( &cross1, &cross2 ) / VectorX3Dot ( &cross2, &cross2 );
	double		t		= VectorX3Dot ( &cross3, &cross2 ) / VectorX3Dot ( &cross2, &cross2 );
	VectorX3	Pos1	= D1 * s + P1;
	VectorX3	Pos2	= D2 * t + P2;
	return (Pos1 + Pos2)*0.5f;
}

__inline
VectorX3		IntersectionCoordLineToPlane	( VectorX3 Pos1, VectorX3 Dir1, VectorX3 Nor, double d )
{
	Dir1.Normalize();
	Nor.Normalize();

	double	D			= Nor.x * Pos1.x + Nor.y * Pos1.y + Nor.z * Pos1.z + d;
	double	cosTheta	= Nor.x * Dir1.x + Nor.y * Dir1.y + Nor.z * Dir1.z;
	double	sinTheta	= fsqr(1.0f - cosTheta * cosTheta);

	double	length		= D / fabs(cosTheta);

	return Pos1 + (Dir1 * length);
}


__inline
Vector4*	Vector4Transform(Vector4* c, const Vector4* a, const Matrix4x4* b)
{
	c->x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * a->w;
	c->y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * a->w;
	c->z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * a->w;
	c->w = b->_14*a->x + b->_24*a->y + b->_34*a->z + b->_44 * a->w;
	c->x /= c->w;
	c->y /= c->w;
	c->z /= c->w;
	return c;
}

__inline
Vector4*	Vector4TransformOffset(Vector4* c, const Vector4* a, const Matrix4x4* b)
{
	c->x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * a->w;
	c->y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * a->w;
	c->z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * a->w;
	c->w = b->_14*a->x + b->_24*a->y + b->_34*a->z + b->_44 * a->w;
	c->x /= c->w;
	c->y /= c->w;
	c->z /= c->w;
	c->x = c->x * 0.5 + 0.5;
	c->y = c->y *-0.5 + 0.5;
	return c;
}

__inline
Vector2	WorldToScreen	(VectorX3* a, const Matrix4x4* b, int viewX, int viewY, int viewW, int viewH )
{
	Vector4	c;
	c.x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * 1;
	c.y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * 1;
	c.z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * 1;
	c.w = b->_14*a->x + b->_24*a->y + b->_34*a->z + b->_44 * 1;
	c.x /= c.w;
	c.y /= c.w;
	c.z /= c.w;
	c.x = (c.x * 0.5 + 0.5) * viewW + viewX;
	c.y = (c.y *-0.5 + 0.5) * viewH + viewY;

	return Vector2(c.x, c.y);
}

__inline
VectorX3	WorldToScreenZ	(VectorX3* a, const Matrix4x4* b, int viewX, int viewY, int viewW, int viewH )
{
	Vector4	c;
	c.x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * 1;
	c.y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * 1;
	c.z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * 1;
	c.w = b->_14*a->x + b->_24*a->y + b->_34*a->z + b->_44 * 1;
	c.x /= c.w;
	c.y /= c.w;
	c.z /= c.w;
	c.x = (c.x * 0.5 + 0.5) * viewW + viewX;
	c.y = (c.y *-0.5 + 0.5) * viewH + viewY;

	return VectorX3(c.x, c.y, c.z);
}

__inline
Vector4	WorldToScreenW	(VectorX3* a, const Matrix4x4* b, int viewX, int viewY, int viewW, int viewH )
{
	Vector4	c;
	c.x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * 1;
	c.y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * 1;
	c.z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * 1;
	c.w = b->_14*a->x + b->_24*a->y + b->_34*a->z + b->_44 * 1;
	c.x /= c.w;
	c.y /= c.w;
	c.z /= c.w;
	c.x = (c.x * 0.5 + 0.5) * viewW + viewX;
	c.y = (c.y *-0.5 + 0.5) * viewH + viewY;

	return c;
}

__inline
VectorX3	WorldToScreen3	(VectorX3* a, const Matrix4x4* b, int viewX, int viewY, int viewW, int viewH )
{
	Vector4	c;
	c.x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * 1;
	c.y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * 1;
	c.z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * 1;
	c.w = b->_14*a->x + b->_24*a->y + b->_34*a->z + b->_44 * 1;
	c.x /= c.w;
	c.y /= c.w;
	c.z /= c.w;
	c.x = (c.x * 0.5 + 0.5) * viewW + viewX;
	c.y = (c.y *-0.5 + 0.5) * viewH + viewY;

	return VectorX3(c.x, c.y, c.z);
}

__inline
VectorX3*	VectorX3Transform(VectorX3* c, const VectorX3* a1, const Matrix4x4* b)
{
	VectorX3	va = *a1;
	VectorX3*	a = &va;
	c->x = b->_11*a->x + b->_21*a->y + b->_31*a->z + b->_41 * 1;
	c->y = b->_12*a->x + b->_22*a->y + b->_32*a->z + b->_42 * 1;
	c->z = b->_13*a->x + b->_23*a->y + b->_33*a->z + b->_43 * 1;
	return c;
}

#define M11  0.0 
#define M12  1.0
#define M13  0.0
#define M14  0.0
#define M21 -0.5
#define M22  0.0
#define M23  0.5
#define M24  0.0
#define M31  1.0
#define M32 -2.5
#define M33  2.0
#define M34 -0.5
#define M41 -0.5
#define M42  1.5
#define M43 -1.5
#define M44  0.5
 
__inline
double catmullRomSpline(float v0,float v1, float v2,float v3, float s)
{
	double c1,c2,c3,c4;

	c1 = M12*v1;
	c2 = M21*v0 + M23*v2;
	c3 = M31*v0 + M32*v1 + M33*v2 + M34*v3;
	c4 = M41*v0 + M42*v1 + M43*v2 + M44*v3;

	return(((c4*s + c3)*s +c2)*s + c1);
}

__inline
Vector2	LensCurrect	( Vector2 pos, Vector2 scl, Vector2 off, double amount = -0.00536/* python300 672X496 1/2.5" 2.5mm lens*/ )
{
	float	p		= amount;
	Vector2	oTex	= (pos - (scl/2) + off)/scl;
	Vector2	t		= (oTex * 2.0)*Vector2(1.0f, 1.0f);
	// Method by H. Farid and A.C. Popescu
	float	x		= t.x;
	float	y		= t.y;
	float	r		= scl.y / scl.x;
	t.y				/= r;
	for ( int i = 0; i < 1; i ++ )
	{
		float	r		= t.Length() * 0.99742651;
		float	r2		= pow(r, 2);
		float	denom	= 1.0f-p*r2;
		Vector2	temp	= (t / denom) * (1.0f-p);
		denom	= 1.0f-p*(temp.x*temp.x+temp.y*temp.y);
		temp	= (t / denom) * (1.0f-p);
		x		= temp.x / 0.99742651;
		y		= temp.y / 0.99742651;
		t		= Vector2(x, y);
	}
	t.y		*= r;
	oTex	= (t*0.5f)+0.5f;
	oTex	*= scl;
	return oTex;
}

__inline
void			Unproj	( float	x, float y, VectorX3* pSrc, VectorX3* pDir, Matrix4x4 matView, Matrix4x4 matProj )
{
	VectorX3		pos, tgt, v;
	Matrix4x4		matIV = matView;

	matIV.Inverse();
	v.x =  ( ( (x * 2) / SIZEW) - 1.0f ) / (matProj._11 * matProj._33);
	v.y = -( ( (y * 2) / SIZEH) - 1.0f ) / (matProj._22 * matProj._33);
	v.z =  1.0f;

	// Transform the screen space pick ray into 3D space
	if ( pSrc != NULL )
	{
		pSrc->x = matIV._41;
		pSrc->y = matIV._42;
		pSrc->z = matIV._43;
	}

	if ( pDir != NULL )
	{
//		v.Normalize();
		pDir->x  = v.x*matIV._11 + v.y*matIV._21 + v.z*matIV._31;
		pDir->y  = v.x*matIV._12 + v.y*matIV._22 + v.z*matIV._32;
		pDir->z  = v.x*matIV._13 + v.y*matIV._23 + v.z*matIV._33;
		pDir->Normalize();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 8 x 8 matrix
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Matrix8x8
{
public:
    union
	{
        struct
		{
            double        _11, _12, _13, _14, _15, _16, _17, _18;
            double        _21, _22, _23, _24, _25, _26, _27, _28;
            double        _31, _32, _33, _34, _35, _36, _37, _38;
            double        _41, _42, _43, _44, _45, _46, _47, _48;
            double        _51, _52, _53, _54, _55, _56, _57, _58;
            double        _61, _62, _63, _64, _65, _66, _67, _68;
            double        _71, _72, _73, _74, _75, _76, _77, _78;
            double        _81, _82, _83, _84, _85, _86, _87, _88;
        };
        double m[8][8];
    };
    Matrix8x8();

	void				CreateFromPositions	( Vector2* src, Vector2* tgt );
	bool				Inverse		( void );
	__inline	void	Identity	( void )
	{
		memset ( m, 0, sizeof(double)*64 );
		_11 = 1, _22 = 1, _33 = 1, _44 = 1, _55 = 1, _66 = 1, _77 = 1, _88 = 1;
	}
};

//--------------------------
// Matrix
//--------------------------
__inline
Matrix8x8*	Matrix8x8Multiply(Matrix8x8* c, const Matrix8x8* a, const Matrix8x8* b)
{
	for ( int i = 0; i < 8; i ++ )
	{
		for ( int j = 0; j < 8; j ++ )
		{
			c->m[i][j] =a->m[i][0] * b->m[0][j] +
						a->m[i][1] * b->m[1][j] +
						a->m[i][2] * b->m[2][j] +
						a->m[i][3] * b->m[3][j] +
						a->m[i][4] * b->m[4][j] +
						a->m[i][5] * b->m[5][j] +
						a->m[i][6] * b->m[6][j] +
						a->m[i][7] * b->m[7][j];
		}
	}
	return c;
}

__inline
Matrix8x8::Matrix8x8()
{
	Identity ();
}

__inline
bool	Matrix8x8::Inverse	( void )
{
	int	col = 8;
	Matrix8x8 temp, comp;
	comp.Identity ();
	memcpy ( temp.m, m, sizeof(double)*64 );

	for (int n = 0; n < col; n++)
	{
		if (temp.m[n][n] == 0) //
		{
			for (int k = n; k < col; k++)
			{
				if (temp.m[k][n] != 0)
				{
					for (int a = 0; a < col; a++)
					{
						comp.m[n][a] = comp.m[n][a] + comp.m[k][a];
						temp.m[n][a] = temp.m[n][a] + temp.m[k][a];
					}
					break;
				}
			}
		}
		double p = temp.m[n][n];
		for (int a = 0; a < col; a++)
		{
			comp.m[n][a] /= p;
			temp.m[n][a] /= p;
		}
		for (int i = 0; i < col; i++)
		{
			if (i != n)
			{
				double p1 = temp.m[i][n];
				for (int j = col - 1; j >= 0; j--)
				{
					comp.m[i][j] -= p1 * comp.m[n][j];
					temp.m[i][j] -= p1 * temp.m[n][j];
				}
			}
		}
	}

	memcpy ( m, comp.m, sizeof(double)*64 );
	return true;
}

__inline
void	Matrix8x8::CreateFromPositions	( Vector2* src, Vector2* tgt )
{
	memset ( m, 0, sizeof(double)*64 );
	m[0][0] = src[0].x;	m[0][1] = src[0].y;m[0][2] = 1;	m[0][6] = -src[0].x*tgt[0].x;	m[0][7] = -src[0].x*tgt[0].y;
	m[1][3] = src[0].x;	m[1][4] = src[0].y;m[1][5] = 1;	m[1][6] = -src[0].x*tgt[0].y;	m[1][7] = -src[0].y*tgt[0].y;
	m[2][0] = src[1].x;	m[2][1] = src[1].y;m[2][2] = 1;	m[2][6] = -src[1].x*tgt[1].x;	m[2][7] = -src[1].x*tgt[1].y;
	m[3][3] = src[1].x;	m[3][4] = src[1].y;m[3][5] = 1;	m[3][6] = -src[1].x*tgt[1].y;	m[3][7] = -src[1].y*tgt[1].y;
	m[4][0] = src[2].x;	m[4][1] = src[2].y;m[4][2] = 1;	m[4][6] = -src[2].x*tgt[2].x;	m[4][7] = -src[2].x*tgt[2].y;
	m[5][3] = src[2].x;	m[5][4] = src[2].y;m[5][5] = 1;	m[5][6] = -src[2].x*tgt[2].y;	m[5][7] = -src[2].y*tgt[2].y;
	m[6][0] = src[3].x;	m[6][1] = src[3].y;m[6][2] = 1;	m[6][6] = -src[3].x*tgt[3].x;	m[6][7] = -src[3].x*tgt[3].y;
	m[7][3] = src[3].x;	m[7][4] = src[3].y;m[7][5] = 1;	m[7][6] = -src[3].x*tgt[3].y;	m[7][7] = -src[3].y*tgt[3].y;
}

__inline
double*	Vector8Transform(double* c, const double* a, const Matrix8x8* b)
{
	c[0] = b->_11*a[0] + b->_21*a[1] + b->_31*a[2] + b->_41 * a[3] + b->_51*a[4] + b->_61*a[5] + b->_71*a[6] + b->_81 * a[7];
	c[1] = b->_12*a[0] + b->_22*a[1] + b->_32*a[2] + b->_42 * a[3] + b->_52*a[4] + b->_62*a[5] + b->_72*a[6] + b->_82 * a[7];
	c[2] = b->_13*a[0] + b->_23*a[1] + b->_33*a[2] + b->_43 * a[3] + b->_53*a[4] + b->_63*a[5] + b->_73*a[6] + b->_83 * a[7];
	c[3] = b->_14*a[0] + b->_24*a[1] + b->_34*a[2] + b->_44 * a[3] + b->_54*a[4] + b->_64*a[5] + b->_74*a[6] + b->_84 * a[7];
	c[4] = b->_15*a[0] + b->_25*a[1] + b->_35*a[2] + b->_45 * a[3] + b->_55*a[4] + b->_65*a[5] + b->_75*a[6] + b->_85 * a[7];
	c[5] = b->_16*a[0] + b->_26*a[1] + b->_36*a[2] + b->_46 * a[3] + b->_56*a[4] + b->_66*a[5] + b->_76*a[6] + b->_86 * a[7];
	c[6] = b->_17*a[0] + b->_27*a[1] + b->_37*a[2] + b->_47 * a[3] + b->_57*a[4] + b->_67*a[5] + b->_77*a[6] + b->_87 * a[7];
	c[7] = b->_18*a[0] + b->_28*a[1] + b->_38*a[2] + b->_48 * a[3] + b->_58*a[4] + b->_68*a[5] + b->_78*a[6] + b->_88 * a[7];
	return c;
}

__inline
double	lengthOfLineToPoint ( VectorX3 x1, VectorX3 x2, VectorX3 xp )
{
	VectorX3	x01		= xp-x1;
	VectorX3	x02		= xp-x2;
	VectorX3	x21		= x2-x1;
	VectorX3	cr		= VectorX3Cross( &x01, &x02 );
	double		d_under	= x21.Length();
	double		d_upper	= cr.Length();
	return (d_upper / d_under);
}

__inline
double	lengthOfLineToPoint2 ( VectorX3 pl, VectorX3 pt, VectorX3 p, VectorX3* pos, VectorX3* dir )
{
	VectorX3	v		= pt-pl;
	VectorX3	plp		= p-pl;
				*pos	= pl + (VectorX3Dot(&v, &plp)/VectorX3Dot(&v, &v)) * v;
				*dir	= (v.Length() < 0.00000001)?VectorX3(0,0,0):(*pos - p);
	return	dir->Length();
}

__inline
Vector4		CreatePlaneFrom3Points	( VectorX3 Pos1, VectorX3 Pos2, VectorX3 Pos3 )
{
	Vector4	ret;
	VectorX3	a = Pos2-Pos1; 
	VectorX3	b = Pos3-Pos2;
	VectorX3	d = VectorX3Cross ( &a, &b );
				d.Normalize();

	double		k = -(d.x*Pos1.x + d.y*Pos1.y + d.z*Pos1.z);
	return Vector4(d.x, d.y, d.z, k);
}

__inline
bool		IntersectionCoord2Planes	( Vector4 p1, Vector4 p2, VectorX3* o, VectorX3* d )
{
	// logically the 3rd plane, but we only use the normal component.
	VectorX3	p1_normal(p1.x, p1.y, p1.z );
	VectorX3	p2_normal(p2.x, p2.y, p2.z );
	p1_normal.Normalize();
	p2_normal.Normalize();
	VectorX3 p3_normal = VectorX3Cross ( &p2_normal, &p1_normal);
	const float det = pow(p3_normal.Length(), 2);

	// If the determinant is 0, that means parallel planes, no intersection.
	if (det != 0)
	{
		// calculate the final (point, normal)
		*o = ((VectorX3Cross( &p3_normal, &p2_normal) * p1.w) +
					(VectorX3Cross( &p1_normal, &p3_normal) * p2.w)) / det;
		*d = p3_normal;
		d->Normalize();
		return true;
	}
	else
	{
		return false;
	}
}

__inline
bool		IntersectionCoord2Planes	( VectorX3* pA, VectorX3* pB, VectorX3* o, VectorX3* d )
{
	// logically the 3rd plane, but we only use the normal component.
	Vector4		p1 = CreatePlaneFrom3Points	( pA[0], pA[1], pA[2] );
	Vector4		p2 = CreatePlaneFrom3Points	( pB[0], pB[1], pB[2] );

	p2.z = (p1.z*0.5+p2.z*0.5);
	VectorX3	p1_normal(p1.x, p1.y, p1.z );
	VectorX3	p2_normal(p2.x, p2.y, p2.z );
	p1_normal.Normalize();
	p2_normal.Normalize();
	VectorX3 p3_normal = VectorX3Cross ( &p2_normal, &p1_normal);
	const float det = pow(p3_normal.Length(), 2);

	// If the determinant is 0, that means parallel planes, no intersection.
	if (det != 0)
	{
		// calculate the final (point, normal)
		*o = ((VectorX3Cross( &p3_normal, &p2_normal) * p1.w) +
					(VectorX3Cross( &p1_normal, &p3_normal) * p2.w)) / det;
		*d = p3_normal;
		d->Normalize();
		return true;
	}
	else
	{
		return false;
	}
}

__inline
VectorX3	calcVerticalVector	( VectorX3 a, VectorX3 b )
{
	VectorX3 Ret = VectorX3(0.0f, 0.0f, 0.0f);
	VectorX3	Vel	= a;
	VectorX3	Nor	= b;
	Nor.Normalize();

	VectorX3	A = ( Vel.x*Nor.x + Vel.y*Nor.y + Vel.z*Nor.z ) * Nor;
	return A;
}

__inline
VectorX3	calcHorizontalVector	( VectorX3 a, VectorX3 b )
{
	VectorX3 Ret = VectorX3(0.0f, 0.0f, 0.0f);
	VectorX3	Vel	= a;
	VectorX3	Nor	= b;
	Nor.Normalize();

	VectorX3	A = ( Vel.x*Nor.x + Vel.y*Nor.y + Vel.z*Nor.z ) * Nor;
	VectorX3	B = Vel - A;

	return B;
}

__inline
VectorX3	calcReflectionVector	( VectorX3 a, VectorX3 b )
{
	VectorX3 Ret = VectorX3(0.0f, 0.0f, 0.0f);
	VectorX3	Vel	= a;
	VectorX3	Nor	= b;
	Nor.Normalize();

	VectorX3	A = ( Vel.x*Nor.x + Vel.y*Nor.y + Vel.z*Nor.z ) * Nor;
	VectorX3	B = Vel - A;

	Ret = (-A+B);

	return Ret;
}

// 0 출력 ; 에러없음, 999 출력 ; 계산에러
double  leastSquare_v2(Vector2 v[], int n, int number, double c[], double* spread = NULL);
double	leastSquare_parabolic ( double* x, double* y, int N, double* c );
__inline
double  leastSquare(double x[], double y[], int n, int number, double c[], int step = 1 )
{
   int  i, j, k, l;
   double  w1, w2, w3, pivot, aik, a[21][22], w[42];

   n = __min(20, n);
   if(n >= number || n < 1 || n > 20)
   {
	   for(i = 0; i < n+1; i++) c[i] = 0;
	   return(-1);
   }
   for(i = 0; i < n*2; i++)
   {
      w1 = 0.0;
      for(j = 0; j < number; j++)
      {
         w2 = w3 = x[j];
         for(k = 0; k < i; k++)	w2 *= w3;
         w1 += w2;
      }
      w[i] = w1;
   }

   // matrix 입력
   for(i = 0; i < n+1; i++)
   {
	   for(j = 0; j < n+1; j++)
	   {
		   l = i + j - 1;
		   a[i][j] = w[l];
	   }
   }
   a[0][0] = number;
   w1 = 0.0;
   for(i = 0; i < number; i++) w1 += y[i];
   a[0][n+1] = w1;

   // sigma(Yi Xi) 계산해서 대입
   double	sum = 0;
   for(i = 0; i < n; i++)
   {
      w1 = 0.0;
      for(j = 0; j < number; j++)
      {
         w2 = w3 = x[j];
         for(k = 0; k < i; k++) w2 *= w3;
         w1 += y[j] * w2;
      }
      a[i+1][n+1] = w1;
   }

   // matrix 계산
   for(k = 0; k < n+1; k++)
   {
      pivot = a[k][k];
      for(j = k; j < n+2; j++)
	  {
		  a[k][j] /= pivot;
		  sum	+= fabs(a[k][j]);
	  }
      for(i = 0; i < n+1; i++)
      {
         if(i != k)
         {
            aik = a[i][k];
            for(j = k; j < n+2; j++) a[i][j] -= aik * a[k][j];
         }
      }
   }

   // 다항식의 계수를 최종적으로 전달 (pass by reference)
   for(i = 0; i < n+1; i++) c[i] = a[i][n+1];
   return(sum);
}

__inline	void	Filtering	( double x[], double y[], int n, int number )
{
	double	c[4];
	double	x1[100], y1[100];
	memcpy ( x1, x, sizeof(double)*number );
	memcpy ( y1, y, sizeof(double)*number );
	leastSquare(y1, x1, n, number, c);
	for ( int i = 0; i < number; i ++ )
	{
		x[i] = c[1] * y1[i] + c[0];
	}
}
void mulTransposed( double src[6][6], double dst[6][6], int order, double *delta = NULL, double scale = 1 );

bool	circleFit_LS	( Vector2 Q[], int  N, Vector2 *center, double  *radius, VectorX3* minR = NULL, VectorX3* maxR = NULL );
BOOL	circleFit_LS2	( Vector2 Q[], int	N, Vector2 *center, double *radius);
bool	get3DFrom2DUseTable		( VectorX3* pOut, Vector2 P1, Vector2 P2 );
bool	get3DFrom2DDirectly		( VectorX3* pOut, Vector2 P1, Vector2 P2 );
double	calcCurvatureInXRange	( double* c, int n, double x0, double x1 );
double	calcCurvatureInXRangeVal( double* c, int n, double* x, double* y, int N );

double	stdev					( double* v, int n, double* maxDiff = NULL);