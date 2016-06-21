#include "sensor.h"
#include "constant.h"

#include <qmath.h>
#include <qdebug.h>

/*********************************************************************/
CSensor::CSensor()
{
}

CSensor::~CSensor()
{
}
/*********************************************************************/
/* Public functions */
void CSensor::SetOrigin( double dLongitude, double dLatitude )
{
	m_dOriginLongitude = dLongitude;
	m_dOriginLatitude = dLatitude;

	m_dOriginLongitude = qDegreesToRadians( m_dOriginLongitude );
	m_dOriginLatitude = qDegreesToRadians( m_dOriginLatitude );
}

//! ���ԁi�����̓_�Q�j�̐��E���W�n�ɂ�����ʒu�����߂�
void CSensor::CalcGlobalPosition( double dLongitude, double dLatitude, double* pdPosX, double* pdPosY )
{
	dLongitude = qDegreesToRadians( dLongitude );
	dLatitude = qDegreesToRadians( dLatitude );

	double dDistance = 0.0;
	double dAzimuth = 0.0;

	//! ���ʊp�̌v�Z
	dAzimuth = qRadiansToDegrees(CalculateAzimuth(m_dOriginLatitude, m_dOriginLongitude, dLatitude, dLongitude));

	if( dAzimuth < 0.0 )
		dAzimuth += 360;

	//! Lambert Andoyer Method
	double A = 6378140;
	double B = 6356755;
	double F = ( A - B ) / A;

	double P1 = qAtan( ( B / A ) * qTan( m_dOriginLatitude ) );
	double P2 = qAtan( ( B / A ) * qTan( dLatitude ) );

	double X = qAcos( qSin( P1 ) * qSin( P2 ) + qCos( P1 ) * qCos( P2 ) * qCos( m_dOriginLongitude - dLongitude ) );
	double L = (F / 8) * ((qSin(X) - X) * qPow((qSin(P1) + qSin(P2)), 2) / qPow(qCos(X / 2), 2) - (qSin(X) + X) * qPow(qSin(P1) - qSin(P2), 2) / qPow(qSin(X / 2), 2));

	dDistance = A * ( X + L ); // UNIT : meter [m]

	double dAzimuthRad = qDegreesToRadians( dAzimuth );

	*pdPosX = dDistance * qSin( dAzimuthRad );
	*pdPosY = dDistance * qCos( dAzimuthRad );
}
#if 0
void CSensor::CalcGlobalPosition( int nTick, double dLongitude, double dLatitude, double* pdPosX, double* pdPosY, double* pdAzimuth )
{
	dLongitude = qDegreesToRadians( dLongitude );
	dLatitude = qDegreesToRadians( dLatitude );

	double dDistance = 0.0;
	double dAzimuth = 0.0;

	//! ���ʊp�̌v�Z
	dAzimuth = qRadiansToDegrees(CalculateAzimuth(m_dOriginLatitude, m_dOriginLongitude, dLatitude, dLongitude));

	if( dAzimuth < 0.0 )
		dAzimuth += 360;

	double A = 6378140;
	double B = 6356755;
	double F = ( A - B ) / A;

	double P1 = qAtan( ( B / A ) * qTan( m_dOriginLatitude ) );
	double P2 = qAtan( ( B / A ) * qTan( dLatitude ) );

	double X = qAcos( qSin( P1 ) * qSin( P2 ) + qCos( P1 ) * qCos( P2 ) * qCos( m_dOriginLongitude - dLongitude ) );
	double L = (F / 8) * ((qSin(X) - X) * qPow((qSin(P1) + qSin(P2)), 2) / qPow(qCos(X / 2), 2) - (qSin(X) - X) * qPow(qSin(P1) - qSin(P2), 2) / qPow(qSin(X), 2));

	dDistance = A * ( X + L ); // UNIT : meter [m]

	double dAzimuthRad = qDegreesToRadians( dAzimuth );

	*pdPosX = dDistance * qSin( dAzimuthRad );
	*pdPosY = dDistance * qCos( dAzimuthRad );
}
#endif

//! ���Ԃ̐��E���W�n�ɂ�����ʒu�����߂�
void CSensor::CalcGlobalPosition( double dOwnGlobalX, double dOwnGlobalY, double dOwnAttitude, double* pdPosX, double* pdPosY  )
{
	//! ���Ԃ̎ԍ��W�n�ɂ�����ʒu
	double dAdjVehX = *pdPosX;
	double dAdjVehY = *pdPosY;
	
	//! ���Ԃ̎p���p(Radian)
	dOwnAttitude = qDegreesToRadians(dOwnAttitude);

	//! �ԍ��W�n����Global���W�n�֕ϊ�����
	double dAdjGlobalX = dAdjVehX * qCos(dOwnAttitude) - dAdjVehY * qSin(dOwnAttitude);
	double dAdjGlobalY = dAdjVehX * qSin(dOwnAttitude) + dAdjVehY * qCos(dOwnAttitude);

	*pdPosX = dAdjGlobalX + dOwnGlobalX;
	*pdPosY = dAdjGlobalY + dOwnGlobalY;
}

double CSensor::CalculateAzimuth(double dLatA, double dLongA, double dLatB, double dLongB)
{
	double dAzimuth = 0.0;

	double A = 6378140; //! �n���ԓ����a [m]
	double B = 6356755; //! �n���ɔ��a [m]

	double dPhiA = qAtan( B / A * qTan(dLatA) );
	double dPhiB = qAtan( B / A * qTan(dLatB) );
	double dH = dLongB - dLongA;

	dAzimuth = atan(qSin(dH) / (qCos(dPhiA)*qTan(dPhiB) - qSin(dPhiA)*qCos(dH)));

	return dAzimuth;
}

double CSensor::CalculateAttitude(double dPrePosX, double dPrePosY, double dPosX, double dPosY, int nCurrentLane, int nLaneMax, void* pvData)
{
	double dAttitude = 0.0;
	double(*pdLaneData)[LINE_MAX_INDEX][2] = reinterpret_cast<double(*)[LINE_MAX_INDEX][2]>(pvData);

	//! ���Ԃ̈ʒu�ƍł��߂������_��T��
	double dMinDst = DBL_MAX;
	int nMinIndex = 0;
	for (int n = 0; n < nLaneMax; n++)
	{
		double dPtX = pdLaneData[nCurrentLane][n][0];
		double dPtY = pdLaneData[nCurrentLane][n][1];

		double dDistance = qSqrt((dPosX - dPtX)*(dPosX - dPtX) + (dPosY - dPtY)*(dPosY - dPtY));

		if( dDistance < dMinDst )
		{
			dMinDst = dDistance;
			nMinIndex = n;
		}
	}

	double dPtX1 = 0.0;
	double dPtY1 = 0.0;

	double dPtX2 = 0.0;
	double dPtY2 = 0.0;

	if( nMinIndex < 5 )
	{
		dPtX1 = pdLaneData[nCurrentLane][nMinIndex][0];
		dPtY1 = pdLaneData[nCurrentLane][nMinIndex][1];

		dPtX2 = pdLaneData[nCurrentLane][nMinIndex + 5][0];
		dPtY2 = pdLaneData[nCurrentLane][nMinIndex + 5][1];
	}
	else if (nMinIndex >(nLaneMax-5))
	{
		dPtX1 = pdLaneData[nCurrentLane][nMinIndex - 5][0];
		dPtY1 = pdLaneData[nCurrentLane][nMinIndex - 5][1];

		dPtX2 = pdLaneData[nCurrentLane][nMinIndex][0];
		dPtY2 = pdLaneData[nCurrentLane][nMinIndex][1];
	}
	else
	{
		dPtX1 = pdLaneData[nCurrentLane][nMinIndex - 5][0];
		dPtY1 = pdLaneData[nCurrentLane][nMinIndex - 5][1];

		dPtX2 = pdLaneData[nCurrentLane][nMinIndex + 5][0];
		dPtY2 = pdLaneData[nCurrentLane][nMinIndex + 5][1];
	}

	//! �����̊p�x���Z�o����
	dAttitude = qRadiansToDegrees(qAtan((dPtY2 - dPtY1) / (dPtX2 - dPtX1)));

	//! ���Ԃ̎p���p�����߂�
	double dAttVeh = qRadiansToDegrees(qAtan((dPosY - dPrePosY) / (dPosX - dPrePosX)));
	//! �i�s�������Z�o����
	if ((dPosY - dPrePosY) < 0 && (dPosX - dPrePosX) < 0)
		dAttitude += 180.0;

	if (dAttitude < 0.0)
		dAttitude += 360;

#if 0
	//! �p���p���Z�o����
	dAttitude = qRadiansToDegrees(qAtan((dPosY - dPrePosY) / (dPosX - dPrePosX)));
	
	//! �i�s�������Z�o����
	if ((dPosY - dPrePosY) < 0 && (dPosX - dPrePosX) < 0)
		dAttitude += 180.0;

	if (dAttitude < 0.0)
		dAttitude += 360;
#endif

	return dAttitude;
}