#include "mathAlg.h"

#include <qmath.h>
/*********************************************************************/
CMathAlg::CMathAlg()
{
}

CMathAlg::~CMathAlg()
{
}
/*********************************************************************/
void CMathAlg::Sort( int num, int mode, double dArray[][3] )
{
	double temp[3] = { 0.0 };

	for (int i=0; i<num; ++i)
	{
		for (int j=i+1; j<num; ++j)
		{
			if ( dArray[i][mode] > dArray[j][mode] )
			{
				temp[0] = dArray[i][0];
				temp[1] = dArray[i][1];
				temp[2] = dArray[i][2];

				dArray[i][0] = dArray[j][0];
				dArray[i][1] = dArray[j][1];
				dArray[i][2] = dArray[j][2];
				
				dArray[j][0] = temp[0];
				dArray[j][1] = temp[1];
				dArray[j][2] = temp[2];
			}
		}
	}
}

void CMathAlg::Approximate( int nDataNum, double dArray[][3], double* co )
{
	int i, j, k;
	double X, Y;
	double A[GAUSS_N][GAUSS_N+1], xx[GAUSS_N];

	double* x;
	x = new double [nDataNum];

	double* y;
	y = new double [nDataNum];

	for( int n=0; n<nDataNum; n++ )
	{
		x[n] = dArray[n][1];
		y[n] = dArray[n][2];
	}

	/*初期化*/
	for( i=0; i<GAUSS_N; i++ )
	{
		for( j=0; j<GAUSS_N+1; j++ )
		{
			A[i][j]=0.0;
		}
	}

	/*ガウスの消去法で解く行列の作成*/
	for( i=0; i<GAUSS_N; i++ )
	{
		for( j=0; j<GAUSS_N; j++ )
		{
			for( k=0; k<nDataNum; k++ )
			{
				A[i][j]+=pow(x[k],i+j);
			}
		}
	}
	for( i=0; i<GAUSS_N; i++ )
	{
		for( k=0; k<nDataNum; k++ )
		{
			A[i][GAUSS_N]+=pow(x[k],i)*y[k];
		}
	}
	/*ガウスの消去法の実行（配列xxは解、すなわち多項式の係数を入れるためのもの）*/
	gauss( A, xx );

	for( int n=0; n<GAUSS_N; n++ )
	{
		co[n] = xx[n];
	}

	delete [] x;
	delete [] y;
}

double CMathAlg::DistanceWrtLane( double x, double y, double resolution, double* coeff )
{
	double dDistance = 0.0;
	double dMin = DBL_MAX;
	int index = 0;

	int nDataNum = (SENSING_RANGE / resolution);

	double* px = new double [nDataNum];
	double* py = new double [nDataNum];

	for( int i=0; i<nDataNum; i++ )
	{
		px[i] = resolution * i;
		py[i] = coeff[2] * px[i] * px[i] + coeff[1] * px[i] + coeff[0];

		double dDistance = qSqrt( (px[i]-x)*(px[i]-x) + (py[i]-y)*(py[i]-y) );

		if( dDistance < dMin )
		{
			dMin = dDistance;
			index = i;
		}
	}

	if( py[index] > 0 && y < py[index] )
		dMin *= -1;

	if( py[index] < 0 && y > py[index] )
		dMin *= -1;

	delete [] px;
	delete [] py;

	return dMin;
}

void CMathAlg::Transformation( double ox, double oy, double ang, double px, double py, double* rx, double* ry )
{
	double _x = px - ox;
	double _y = py - oy;

	double rad = qDegreesToRadians( ang );

	*rx = _x * qCos( rad ) - _y * qSin( rad );
	*ry = _x * qSin( rad ) + _y * qCos( rad );
}
/*********************************************************************/
/* Private member functions */
void CMathAlg::gauss(double a[GAUSS_N][GAUSS_N+1], double p[GAUSS_N])
{
	int i,j,k,l,pivot;
	double x[GAUSS_N];
	double pp,q,m,b[1][GAUSS_N+1];

	for(i=0;i<GAUSS_N;i++)
	{
		m=0;
		pivot=i;

		for(l=i;l<GAUSS_N;l++)
		{
			if(qAbs(a[l][i])>m) {   //i列の中で一番値が大きい行を選ぶ
				m=qAbs(a[l][i]);
				pivot=l;
			}
		}

		if(pivot!=i) {                          //pivotがiと違えば、行の入れ替え
			for(j=0;j<GAUSS_N+1;j++) {
				b[0][j]=a[i][j];        
				a[i][j]=a[pivot][j];
				a[pivot][j]=b[0][j];
			}
		}
	}

	for(k=0;k<GAUSS_N;k++)
	{
		pp=a[k][k];              //対角要素を保存
		a[k][k]=1;              //対角要素は１になることがわかっているから

		for(j=k+1;j<GAUSS_N+1;j++) {
			a[k][j]/=pp;
		}

		for(i=k+1;i<GAUSS_N;i++) {
			q=a[i][k];

			for(j=k+1;j<GAUSS_N+1;j++) {
				a[i][j]-=q*a[k][j];
			}
			a[i][k]=0;              //０となることがわかっているところ
		}
	}

	//解の計算
	for(i=GAUSS_N-1;i>=0;i--) {
		x[i]=a[i][GAUSS_N];
		for(j=GAUSS_N-1;j>i;j--) {
			x[i]-=a[i][j]*x[j];
		}
	}

	for( int n=0; n<GAUSS_N; n++ )
		p[n] = x[n];

	/*printf("解は\n");
	for(i=0;i<GAUSS_N;i++)
	{
		printf("%f\n",x[i]);
	}*/
}