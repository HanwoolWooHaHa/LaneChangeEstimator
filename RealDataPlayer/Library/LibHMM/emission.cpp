#include "emission.h"
#include "../../constant.h"

#include <qmath.h>
#include <qdebug.h>

double CEmission::GetEmisProbability( int nState, int nTick, stHMM* pModel, void* pData, int nIndex )
{
	double (*pdFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>(pData);

	double AVG[FEATURE_VECTOR_DIMENSION] = { 0.0 };
    double STD[FEATURE_VECTOR_DIMENSION] = { 0.0 };

    for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
    {
        AVG[k] = pModel->EMIS[nState][k][0];
        STD[k] = qSqrt( pModel->EMIS[nState][k][1] );
    }

    double dExp = 1.0;

	for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
    {
        double dValue = pdFeatureData[nIndex][nTick][k];
        
		dExp *= gaussianProbabilityDistribution( dValue, AVG[k], STD[k] );
    }

	return dExp;
}

double CEmission::gaussianProbabilityDistribution( double dValue, double dAvg, double dStd )
{
	static const double INV_SQRT_2PI = 1 / qSqrt( 2 * M_PI );
	double dReturn = 0.0;

	double dNormal = ( dValue - dAvg ) / dStd;
	dReturn = INV_SQRT_2PI / dStd * qExp( -0.5 * dNormal * dNormal );

	if( dReturn > 1.0 )
		dReturn = 1.0;
	else if( dReturn < 0.0 )
		qDebug() << "emission.cpp @ GPD Value was smaller than zero!";

	return dReturn;
}