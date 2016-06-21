#pragma once

class CNavigationMethod
{
public:
	virtual void Initialize(void) = 0;
	virtual void CalculateAccelerate(int nIndex, int nTick, double* pdAccX, double* pdAccY) = 0;
};