#include "lineExtractor.h"
#include "constant.h"

#include <qmath.h>
#include <qdebug.h>

CLineExtractor::CLineExtractor()
{
	m_ppdCoefficient = NULL;

	m_ppdCoefficient = new double *[NUM_LANE];
	for (int n = 0; n<NUM_LANE; n++)
	{
		m_ppdCoefficient[n] = new double[3];
	}
}

CLineExtractor::~CLineExtractor()
{
	//! 近似曲線のメモリを開放する
	deleteCoefficientData();
}

int CLineExtractor::Extract(int nLine, int nNumMarks, double** ppdData, double* pdCoeff)
{
	approximate(nNumMarks, ppdData, pdCoeff);
	//qDebug() << "lineExtractor.cpp @ Line" << QString::number(n+1) << " : a0=" << pdCoeff[0] << ", a1=" << pdCoeff[1] << ", a2=" << pdCoeff[2];
	
	m_ppdCoefficient[nLine][0] = pdCoeff[0];
	m_ppdCoefficient[nLine][1] = pdCoeff[1];
	m_ppdCoefficient[nLine][2] = pdCoeff[2];

	return 0;
}

void CLineExtractor::SetCoefficients(double(*pdCoeff)[3])
{
	if( m_ppdCoefficient != NULL )
		return;

	m_ppdCoefficient = new double *[NUM_LANE];
	for (int n = 0; n<NUM_LANE; n++)
	{
		m_ppdCoefficient[n] = new double [3];

		m_ppdCoefficient[n][0] = pdCoeff[n][0];
		m_ppdCoefficient[n][1] = pdCoeff[n][1];
		m_ppdCoefficient[n][2] = pdCoeff[n][2];

		qDebug() << "lineExtractor.cpp @ Line" << QString::number(n+1) << " : a0=" << m_ppdCoefficient[n][0] << ", a1=" << m_ppdCoefficient[n][1] << ", a2=" << m_ppdCoefficient[n][2];
	}
}

double CLineExtractor::DistanceWrtLine( double dPosX, double dPosY, int nLine, int& nSide )
{
	double dDistance = 0.0;
	double dMin = DBL_MAX;
	double dValue = 0.0;

	int nDataNum = 2 * SENSING_RANGE / RESOLUTION_LINE_POINT;

	double* px = new double [nDataNum];
	double* py = new double [nDataNum];

	double coeff[3] = { m_ppdCoefficient[nLine][0], m_ppdCoefficient[nLine][1], m_ppdCoefficient[nLine][2] };

	for( int i=0; i<nDataNum; i++ )
	{
		px[i] = RESOLUTION_LINE_POINT * i - SENSING_RANGE;
		py[i] = coeff[2] * px[i] * px[i] + coeff[1] * px[i] + coeff[0];

		double dDistance = qSqrt( (px[i]-dPosX)*(px[i]-dPosX) + (py[i]-dPosY)*(py[i]-dPosY) );

		if( dDistance < dMin )
		{
			dMin = dDistance;
			dValue = coeff[2] * dPosX * dPosX + coeff[1] * dPosX + coeff[0] - dPosY;

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
void CLineExtractor::approximate( int nDataNum, double** dArray, double* co )
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
		x[n] = dArray[n][0];
		y[n] = dArray[n][1];
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
	for (int n = 0; n<NUM_LANE; n++)
	{
		delete [] m_ppdCoefficient[n];
	}
	delete [] m_ppdCoefficient;
	m_ppdCoefficient = NULL;
}