#pragma once

#include "../navigationMethod.h"

class CWolfPF : public CNavigationMethod
{
public:
	CWolfPF();
	~CWolfPF();

	virtual void Initialize(void);
	virtual void CalculateAccelerate(int nIndex, int nTick, double* pdAccX, double* pdAccY);

private:
	double calcLanePotential(double dOwnPosY);
	double calcRoadPotential(double dOwnPosY);
	double calcCarPotential(int nIndex, int nTick, double dOwnPosX, double dOwnPosY, double dOwnVelX, double dOwnVelY);
	double calcPseudoDistanceK(int nIndex, int nTick, double dOwnPosX, double dOwnPosY, double dOwnVelX, double dOwnVelY);
	double calcVelocityPotential(double dOwnPosX, double dOwnVelX, double dVelDes);

	double m_dVelDes;
	double m_dDelta;
};