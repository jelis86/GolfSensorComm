#include "Mathematics.h"

VectorX3	Vector2::unprojectOrthoSphere(double targetRadius, bool bBackFace)
{
	VectorX3	ret;
	Vector2		self	= *this;
	double		len		= self.Length();
	len = len > 1.0f ? (2.0f*targetRadius) - len : len;

	ret.x	= self.x;
	ret.y	= self.y;
	ret.z	= sqrt(1.0-(self.x*self.x+self.y*self.y))*(bBackFace==true?-1.0:1.0);
	ret.Normalize();
	return ret*targetRadius;//*(bBackFace==true?-1.0:1.0);
}


bool  circleFit_LS  (Vector2 P[], int  N, Vector2 *center, double  *radius, VectorX3* minR, VectorX3* maxR )
{
	/* user-selected parameters */
	const int maxIterations = 256;
	const double tolerance = 1e-06;

	double a, b, r;

	/* compute the average of the data points */
	int i, j;
	double xAvr = 0.0;
	double yAvr = 0.0;

	for (i = 0; i < N; i++)
	{
		xAvr += P[i].x;
		yAvr += P[i].y;
	}
	xAvr /= N;
	yAvr /= N;

	/* initial guess */
	a = xAvr;
	b = yAvr;

	for (j = 0; j < maxIterations; j++)
	{
		/* update the iterates */
		double a0 = a;
		double b0 = b;

		/* compute average L, dL/da, dL/db */
		double LAvr = 0.0;
		double LaAvr = 0.0;
		double LbAvr = 0.0;
		if ( maxR != NULL )	maxR->z = 0.0;
		if ( minR != NULL )	minR->z = 10000.0;

		for (i = 0; i < N; i++)
		{
			double dx = P[i].x - a;
			double dy = P[i].y - b;
			double L = sqrt(dx * dx + dy * dy);
			if ( maxR != NULL )
			{
				maxR->z = __max(maxR->z, L);
				maxR->x = dx;
				maxR->y = dy;
			}
			if ( minR != NULL )
			{
				minR->z = __min(minR->z, L);
				maxR->x = dx;
				maxR->y = dy;
			}
			if (fabs(L) > tolerance)
			{
				LAvr += L;
				LaAvr -= dx / L;
				LbAvr -= dy / L;
			}
		}
		LAvr /= N;
		LaAvr /= N;
		LbAvr /= N;

		a = xAvr + LAvr * LaAvr;
		b = yAvr + LAvr * LbAvr;
		r = LAvr;

		if (fabs(a - a0) <= tolerance && fabs(b - b0) <= tolerance)
			break;
	}

	center->x = a;
	center->y = b;
	*radius = r;

	return (j < maxIterations ? j : -1);
}
double  leastSquare_v2(Vector2 v[], int n, int number, double c[], double* spread)
{
   int  i, j, k, l;
   double  w1, w2, w3, pivot, aik, a[21][22], w[42];

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
         w2 = w3 = v[j].y;
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
   for(i = 0; i < number; i++) w1 += v[i].x;
   a[0][n+1] = w1;

   // sigma(Yi Xi) 계산해서 대입
   double	sum = 0;
   for(i = 0; i < n; i++)
   {
      w1 = 0.0;
      for(j = 0; j < number; j++)
      {
         w2 = w3 = v[j].y;
         for(k = 0; k < i; k++) w2 *= w3;
         w1 += v[j].x * w2;
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
   if ( spread != NULL )	*spread = pivot;
   return(sum);
}

double	leastSquare_parabolic ( double* x, double* y, int N, double* c )
{
	int i,j,k, n = 2;
    double X[41];                        //Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    for (i=0;i<2*n+1;i++)
    {
        X[i]=0;
        for (j=0;j<N;j++)
            X[i]=X[i]+pow(x[j],i);        //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    }
    double B[21][22],a[21];            //B is the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
    for (i=0;i<=n;i++)
        for (j=0;j<=n;j++)
            B[i][j]=X[i+j];            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
    double Y[21];                    //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    for (i=0;i<n+1;i++)
    {    
        Y[i]=0;
        for (j=0;j<N;j++)
        Y[i]=Y[i]+pow(x[j],i)*y[j];        //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    }
    for (i=0;i<=n;i++)
        B[i][n+1]=Y[i];                //load the values of Y as the last column of B(Normal Matrix but augmented)
    n=n+1;					          //n is made n+1 because the Gaussian Elimination part below was for n equations, but here n is the degree of polynomial and for n degree we get n+1 equations
    for (i=0;i<n;i++)                    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
        for (k=i+1;k<n;k++)
            if (B[i][i]<B[k][i])
                for (j=0;j<=n;j++)
                {
                    double temp=B[i][j];
                    B[i][j]=B[k][j];
                    B[k][j]=temp;
                }
     
    for (i=0;i<n-1;i++)            //loop to perform the gauss elimination
        for (k=i+1;k<n;k++)
            {
                double t=B[k][i]/B[i][i];
                for (j=0;j<=n;j++)
                    B[k][j]=B[k][j]-t*B[i][j];    //make the elements below the pivot elements equal to zero or elimnate the variables
            }
    for (i=n-1;i>=0;i--)                //back-substitution
    {                        //x is an array whose values correspond to the values of x,y,z..
        a[i]=B[i][n];                //make the variable to be calculated equal to the rhs of the last equation
        for (j=0;j<n;j++)
            if (j!=i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
                a[i]=a[i]-B[i][j]*a[j];
        a[i]=a[i]/B[i][i];            //now finally divide the rhs by the coefficient of the variable to be calculated
    }
    for (i=0;i<n;i++) c[i] = a[i];
    return 0;
}

BOOL circleFit_LS2(Vector2 Q[], int N, Vector2 *center, double *radius)
{
    double sx  = 0.0,  sy = 0.0;
    double sx2 = 0.0, sy2 = 0.0, sxy  = 0.0;
    double sx3 = 0.0, sy3 = 0.0, sx2y = 0.0, sxy2 = 0.0;
    /* compute moments; */
    for (int k = 0; k < N; k++)
	{
        double x = Q[k].x, xx = x * x;
        double y = Q[k].y, yy = y * y;
        sx   += x;       sy   += y;
        sx2  += xx;      sy2  += yy;      sxy  += x * y;
        sx3  += x * xx;  sy3  += y * yy;
        sx2y += xx * y;  sxy2 += yy * x;
    }
    /* compute a's,b's,c's; */
    double a1 = 2.0 * (sx * sx - sx2 * N);
    double a2 = 2.0 * (sx * sy - sxy * N);
    double b1 = a2;
    double b2 = 2.0 * (sy * sy - sy2 * N);
    double c1 = (sx2 + sy2) * sx - (sx3 * N + sxy2) * N;
    double c2 = (sx2 + sy2) * sy - (sy3 * N + sx2y) * N;
    double det = a1 * b2 - a2 * b1;
    if (fabs(det) < 1.e-10)                /*collinear한 경우임;*/
        return FALSE;
    /* floating point center; */
    double cx = (c1 * b2 - c2 * b1) / det;
    double cy = (a1 * c2 - a2 * c1) / det;
    /* compute radius squared */
    double radsq = cx * cx + cy * cy + (sx2 + sy2 - 2 * (sx * cx  + sy * cy)) / N;
    *radius = sqrt(radsq);
    /* integer center; */
    center->x = int(cx + .5);
    center->y = int(cy + .5);
    return TRUE;
}

double	calcNFunc				( double* c, int n, double x )
{
	double	y = 0;
	for ( int i = 0; i <= n; i ++ )
	{
		y += c[i] * pow( x, i);
	}
	return y;
}

double	calcCurvatureInXRangeVal( double* c, int n, double* x, double* y, int N )
{
	double	curvature = 0.0;
	for ( int i = 0; i < N; i ++ )
	{
		Vector2	prev(x[i], calcNFunc(c, n, x[i]) );
		Vector2	next(x[i], y[i]);
		Vector2	sub = prev-next;
		curvature += sub.Length();
	}
	return curvature/N;
}

double	calcCurvatureInXRange	( double* c, int n, double x0, double x1 )
{
	double	curvature = 0.0;

	Vector2	lt( 100, 0);
	Vector2	rt(-100, 0);
	double	idx = 0;
	for ( double i = -1.1; i < 1.1; i +=0.01, idx++ )
	{
		Vector2	prev(i, calcNFunc(c, n, i) ), next, sub;
		if ( prev.Length() > 1.0 )	continue;
		lt = prev.x < lt.x ? prev : lt;
		rt = prev.x > rt.x ? prev : rt;
		idx++;
	}
	double	a0	= (lt.y-rt.y)/(lt.x-rt.x);
	double	b0	= lt.y - a0*lt.x;
	double	fa	= (lt.y-rt.y);
	double	fb	= (rt.x-lt.x);
	double	fc	= (lt.x-rt.x)*b0;
	double	interval = (rt.x-lt.x)*0.01;

	Vector2	pt((lt.x+rt.x)*0.5, calcNFunc(c, n, (lt.x+rt.x)*0.5) );
	Vector2	nt, ct;
	nt.x = pt.x - ((2.0*fa*(fa*pt.x+fb*pt.y+fc)) / (fa*fa+fb*fb));
	nt.y = pt.y - ((2.0*fb*(fa*pt.x+fb*pt.y+fc)) / (fa*fa+fb*fb));
	ct = (nt+pt)*0.5;
	Vector2	h = ct-pt;
	Vector2	w = lt-rt;
	curvature = (((h.Length() * w.Length())*0.5) * 4.0) / 3.0;
	return curvature / w.Length();
}

double	stdev					( double* v, int n, double* maxDiff )
{
	if ( n < 2 )	return -1;

	double	sum = 0, avr, var = 0;
	for ( int i = 0; i < n; i ++ )
	{
		sum += v[i];
	}
	avr = sum/(double)n;

	if ( maxDiff != NULL )	*maxDiff = 0.0;
	for ( int i = 0; i < n; i ++ )
	{
		double diff = pow(avr-v[i], 2);
		if ( maxDiff != NULL )	*maxDiff = __max(*maxDiff, diff);
		var += diff;
	}
	var /= (double)(n-1);

	if ( maxDiff != NULL )	*maxDiff = sqrt(*maxDiff);
	return sqrt(var);
}
