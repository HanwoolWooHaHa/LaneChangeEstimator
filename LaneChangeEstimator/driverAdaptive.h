/**
* @class	CDriverAdaptive
* @author	Hanwool Woo
* @version	1.0
* @date	Creation date: 2016/07/12
* @brief	this class is for driver-adaptive method
*/

#pragma once

class CDriverAdaptive
{
public:
	CDriverAdaptive();
	~CDriverAdaptive();

	static void Adjust(int nDataNo, int nDataLength, void* pvData);
	static double dAvgStandardDeviation[2];

private:
	static double m_dStandardDeviation[2];
	
};