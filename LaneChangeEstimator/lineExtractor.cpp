#include "lineExtractor.h"
#include "defines.h"

#include <qmath.h>
#include <qdebug.h>

CLineExtractor::CLineExtractor()
{
	m_ppdCoefficient = NULL;
}

CLineExtractor::~CLineExtractor()
{
	deleteCoefficientData();
}

int CLineExtractor::Extract( int* pnNumPt, double (*pdData)[500][2], double (*pdCoeff)[3] )
{
	for( int n=0; n<NUM_LINE; n++ )
	{
		approximate( pnNumPt[n], pdData[n], pdCoeff[n] );
		qDebug() << "lineExtractor.cpp @ Line" << QString::number(n+1) << " : a0=" << pdCoeff[n][0] << ", a1=" << pdCoeff[n][1] << ", a2=" << pdCoeff[n][2];
	}

	if( m_ppdCoefficient != NULL )
		deleteCoefficientData();

	m_ppdCoefficient = new double * [NUM_LINE];
	for( int n=0; n<NUM_LINE; n++ )
	{
		m_ppdCoefficient[n] = new double [3];

		m_ppdCoefficient[n][0] = pdCoeff[n][0];
		m_ppdCoefficient[n][1] = pdCoeff[n][1];
		m_ppdCoefficient[n][2] = pdCoeff[n][2];
	}

	return DONE;
}

void CLineExtractor::Set( double (*pdCoeff)[3] )
{
	if( m_ppdCoefficient != NULL )
		return;

	m_ppdCoefficient = new double * [NUM_LINE];
	for( int n=0; n<NUM_LINE; n++ )
	{
		m_ppdCoefficient[n] = new double [3];

		m_ppdCoefficient[n][0] = pdCoeff[n][0];
		m_ppdCoefficient[n][1] = pdCoeff[n][1];
		m_ppdCoefficient[n][2] = pdCoeff[n][2];

		qDebug() << "lineExtractor.cpp @ Line" << QString::number(n+1) << " : a0=" << m_ppdCoefficient[n][0] << ", a1=" << m_ppdCoefficient[n][1] << ", a2=" << m_ppdCoefficient[n][2];
	}
}

double CLineExtractor::DistanceWrtLane( double dPosX, double dPosY, int nLane, int& nSide )
{
	double dDistance = 0.0;
	double dMin = DBL_MAX;
	double dValue = 0.0;

	int nDataNum = MAX_Y / RESOLUTION_LINE_PINT;

	double* px = new double [nDataNum];
	double* py = new double [nDataNum];

	double coeff[3] = { m_ppdCoefficient[nLane][0], m_ppdCoefficient[nLane][1], m_ppdCoefficient[nLane][2] };

	for( int i=0; i<nDataNum; i++ )
	{
		py[i] = RESOLUTION_LINE_PINT * i;
		px[i] = coeff[2] * py[i] * py[i] + coeff[1] * py[i] + coeff[0];

		double dDistance = qSqrt( (px[i]-dPosX)*(px[i]-dPosX) + (py[i]-dPosY)*(py[i]-dPosY) );

		if( dDistance < dMin )
		{
			dMin = dDistance;
			dValue = coeff[2] * dPosY * dPosY + coeff[1] * dPosY + coeff[0] - dPosX;

			if( dValue >= 0.0 ) nSide = 1;
			else nSide = -1;
		}
	}

	delete [] px;
	delete [] py;

	return dMin;
}
/*********************************************************************/
/* Private member functions */
void CLineExtractor::approximate( int nDataNum, double dArray[][2], double* co )
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
		y[n] = dArray[n][0];
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

void CLineExtractor::gauss(double a[GAUSS_N][GAUSS_N+1], double p[GAUSS_N])
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
}

void CLineExtractor::deleteCoefficientData( void )
{
	for( int n=0; n<NUM_LINE; n++ )
	{
		delete [] m_ppdCoefficient[n];
	}
	delete [] m_ppdCoefficient;
}