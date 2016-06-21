#pragma once

#include "../navigationMethod.h"

class CNormalPF : public CNavigationMethod
{
public:
	CNormalPF();
	~CNormalPF();

	virtual void Initialize(void);
	virtual void CalculateAccelerate(int nIndex, int nTick, double* pdAccX, double* pdAccY);

private:
	double calcPotential(int nIndex, int nTick, double dOwnPosX, double dOwnPosY);
};