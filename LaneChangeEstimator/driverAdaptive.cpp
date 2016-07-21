/**
* @file	driverAdaptive.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/07/12
* @brief	this file is the implementation of driver-adaptive method
*/

#include "driverAdaptive.h"

#include <qmath.h>

double CDriverAdaptive::m_dStandardDeviation[2] = { 0.0 };
double CDriverAdaptive::dAvgStandardDeviation[2] = { 0.0 };
double CDriverAdaptive::m_dReEstimatedAverage[NUM_TRAFFIC_DATA][FEATURE_VECTOR_DIMENSION] = { 0.0 };

CDriverAdaptive::CDriverAdaptive()
{
	
}

CDriverAdaptive::~CDriverAdaptive()
{

}
//====================================================================//
#if 0
void CDriverAdaptive::Adjust(int nDataNo, int nDataLength, void* pvData)
{
	double(*pdFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>(pvData);

	int nTrainingPeriod = nDataLength;

	double dSum[FEATURE_VECTOR_DIMENSION] = { 0.0 };
	double dAvg[FEATURE_VECTOR_DIMENSION] = { 0.0 };
	double dStd[FEATURE_VECTOR_DIMENSION] = { 0.0 };

	//! Calculate a standard deviation for 8 sec.
	for (int t = 0; t < nTrainingPeriod; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		dSum[FEATURE_PACKET_DISTANCE] += dDistance;
		dSum[FEATURE_PACKET_LAT_VELOCITY] += dVelocity;
	}

	dAvg[FEATURE_PACKET_DISTANCE] = dSum[FEATURE_PACKET_DISTANCE] / nTrainingPeriod;
	dAvg[FEATURE_PACKET_LAT_VELOCITY] = dSum[FEATURE_PACKET_LAT_VELOCITY] / nTrainingPeriod;

	dSum[FEATURE_PACKET_DISTANCE] = dSum[FEATURE_PACKET_LAT_VELOCITY] = 0.0;

	for (int t = 0; t < nTrainingPeriod; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		dSum[FEATURE_PACKET_DISTANCE] += (dDistance - dAvg[FEATURE_PACKET_DISTANCE]) * (dDistance - dAvg[FEATURE_PACKET_DISTANCE]);
		dSum[FEATURE_PACKET_LAT_VELOCITY] += (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]) * (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]);
	}

	dStd[FEATURE_PACKET_DISTANCE] = qSqrt(dSum[FEATURE_PACKET_DISTANCE] / nTrainingPeriod);
	dStd[FEATURE_PACKET_LAT_VELOCITY] = qSqrt(dSum[FEATURE_PACKET_LAT_VELOCITY] / nTrainingPeriod);

	double dAdjustStd1 = dStd[FEATURE_PACKET_DISTANCE];
	double dAdjustStd2 = dStd[FEATURE_PACKET_LAT_VELOCITY];

	m_dStandardDeviation[0] += dStd[FEATURE_PACKET_DISTANCE];
	m_dStandardDeviation[1] += dStd[FEATURE_PACKET_LAT_VELOCITY];
}
#endif

#if 0
void CDriverAdaptive::Adjust(int nDataNo, int nDataLength, void* pvData)
{
	m_dStandardDeviation[0] = 0.146171; //0.175307; //0.0991196; //
	m_dStandardDeviation[1] = 0.21937; //0.176952; //0.141877; //

	double(*pdFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>(pvData);

	int nTrainingPeriod = 300;

	double dSum[FEATURE_VECTOR_DIMENSION] = { 0.0 };
	double dAvg[FEATURE_VECTOR_DIMENSION] = { 0.0 };
	double dStd[FEATURE_VECTOR_DIMENSION] = { 0.0 };

	//! Calculate a standard deviation for 8 sec.
	for (int t = 0; t < nTrainingPeriod; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		dSum[FEATURE_PACKET_DISTANCE] += dDistance;
		dSum[FEATURE_PACKET_LAT_VELOCITY] += dVelocity;
	}

	dAvg[FEATURE_PACKET_DISTANCE] = dSum[FEATURE_PACKET_DISTANCE] / nTrainingPeriod;
	dAvg[FEATURE_PACKET_LAT_VELOCITY] = dSum[FEATURE_PACKET_LAT_VELOCITY] / nTrainingPeriod;

	dSum[FEATURE_PACKET_DISTANCE] = dSum[FEATURE_PACKET_LAT_VELOCITY] = 0.0;

	for (int t = 0; t < nTrainingPeriod; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		dSum[FEATURE_PACKET_DISTANCE] += (dDistance - dAvg[FEATURE_PACKET_DISTANCE]) * (dDistance - dAvg[FEATURE_PACKET_DISTANCE]);
		dSum[FEATURE_PACKET_LAT_VELOCITY] += (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]) * (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]);
	}

	dStd[FEATURE_PACKET_DISTANCE] = qSqrt(dSum[FEATURE_PACKET_DISTANCE] / nTrainingPeriod);
	dStd[FEATURE_PACKET_LAT_VELOCITY] = qSqrt(dSum[FEATURE_PACKET_LAT_VELOCITY] / nTrainingPeriod);

	double dAdjustStd1 = dStd[FEATURE_PACKET_DISTANCE];
	double dAdjustStd2 = dStd[FEATURE_PACKET_LAT_VELOCITY];

	dAvgStandardDeviation[0] += dAdjustStd1;
	dAvgStandardDeviation[1] += dAdjustStd2;

	for(int t=0; t<nTrainingPeriod; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		double dRate1 = qAbs(m_dStandardDeviation[0] / dAdjustStd1);
		double dRate2 = qAbs(m_dStandardDeviation[1] / dAdjustStd2);

		//pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE] = dAvg[FEATURE_PACKET_DISTANCE] + dRate1 * (dDistance - dAvg[FEATURE_PACKET_DISTANCE]);
		//pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY] = dAvg[FEATURE_PACKET_LAT_VELOCITY] + dRate2 * (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]);
	}

	dSum[FEATURE_PACKET_DISTANCE] = dAvg[FEATURE_PACKET_DISTANCE] * nTrainingPeriod;
	dSum[FEATURE_PACKET_LAT_VELOCITY] = dAvg[FEATURE_PACKET_LAT_VELOCITY] * nTrainingPeriod;

	for (int t = nTrainingPeriod; t < nDataLength; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		int nNumData = t + 1;

		dSum[FEATURE_PACKET_DISTANCE] += dDistance;
		dSum[FEATURE_PACKET_LAT_VELOCITY] += dVelocity;

		dAvg[FEATURE_PACKET_DISTANCE] = dSum[FEATURE_PACKET_DISTANCE] / nNumData;
		dAvg[FEATURE_PACKET_LAT_VELOCITY] = dSum[FEATURE_PACKET_LAT_VELOCITY] / nNumData;

		double dNewAdjustStd1 = dAdjustStd1 * dAdjustStd1 * (nNumData - 1) + (dDistance - dAvg[FEATURE_PACKET_DISTANCE]) * (dDistance - dAvg[FEATURE_PACKET_DISTANCE]);
		double dNewAdjustStd2 = dAdjustStd2 * dAdjustStd2 * (nNumData - 1) + (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]) * (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]);
		
		dNewAdjustStd1 = qSqrt( dNewAdjustStd1 / nNumData);
		dNewAdjustStd2 = qSqrt( dNewAdjustStd2 / nNumData);

		double dRate1 = qAbs(m_dStandardDeviation[0] / dNewAdjustStd1);
		double dRate2 = qAbs(m_dStandardDeviation[1] / dNewAdjustStd2);

		pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE] = dDistance / dAvg[FEATURE_PACKET_DISTANCE]; //+ dRate1 * (dDistance - dAvg[FEATURE_PACKET_DISTANCE]);
		pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY] = dVelocity / dAvg[FEATURE_PACKET_LAT_VELOCITY]; //+ dRate2 * (dVelocity - dAvg[FEATURE_PACKET_LAT_VELOCITY]);

		dAdjustStd1 = dNewAdjustStd1;
		dAdjustStd2 = dNewAdjustStd2;
	}
}
#endif

void CDriverAdaptive::Adjust(int nDataNo, int nDataLength, void* pvData)
{
	double(*pdFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>(pvData);

	int nTrainingPeriod = MEASUREMENT_TIME - 50;

	double dSum[FEATURE_VECTOR_DIMENSION] = { 0.0 };
	double dAvg[FEATURE_VECTOR_DIMENSION] = { 0.0 };
	double dStd[FEATURE_VECTOR_DIMENSION] = { 0.0 };

	//! Calculate a standard deviation for 8 sec.
	for (int t = 0; t < nTrainingPeriod; t++)
	{
		double dDistance = pdFeatureData[nDataNo][t][FEATURE_PACKET_DISTANCE];
		double dVelocity = pdFeatureData[nDataNo][t][FEATURE_PACKET_LAT_VELOCITY];

		dSum[FEATURE_PACKET_DISTANCE] += dDistance;
		dSum[FEATURE_PACKET_LAT_VELOCITY] += dVelocity;
	}

	dAvg[FEATURE_PACKET_DISTANCE] = dSum[FEATURE_PACKET_DISTANCE] / nTrainingPeriod;
	dAvg[FEATURE_PACKET_LAT_VELOCITY] = dSum[FEATURE_PACKET_LAT_VELOCITY] / nTrainingPeriod;

	m_dReEstimatedAverage[nDataNo][FEATURE_PACKET_DISTANCE] = dAvg[FEATURE_PACKET_DISTANCE];
	m_dReEstimatedAverage[nDataNo][FEATURE_PACKET_LAT_VELOCITY] = dAvg[FEATURE_PACKET_LAT_VELOCITY];
}