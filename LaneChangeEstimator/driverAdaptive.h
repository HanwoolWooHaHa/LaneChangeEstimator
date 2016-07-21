/**
* @class	CDriverAdaptive
* @author	Hanwool Woo
* @version	1.0
* @date	Creation date: 2016/07/12
* @brief	this class is for driver-adaptive method
*/

#pragma once

#include "defines.h"

class CDriverAdaptive
{
public:
	CDriverAdaptive();
	~CDriverAdaptive();

	static void Adjust(int nDataNo, int nDataLength, void* pvData);
	static double dAvgStandardDeviation[2];
	static double GetReEstimatedAverage(int nDataNo, int nFeatureDimension)
	{
		return m_dReEstimatedAverage[nDataNo][nFeatureDimension];
	}

private:
	static double m_dStandardDeviation[2];
	static double m_dReEstimatedAverage[NUM_TRAFFIC_DATA][FEATURE_VECTOR_DIMENSION];
};