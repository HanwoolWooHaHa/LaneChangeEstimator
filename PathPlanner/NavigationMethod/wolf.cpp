#include "wolf.h"
#include "../database.h"

#include <qmath.h>

CWolfPF::CWolfPF()
{

}

CWolfPF::~CWolfPF()
{

}

void CWolfPF::Initialize(void)
{
	m_dVelDes = 19;
	m_dDelta = 0.0001;
}

void CWolfPF::CalculateAccelerate(int nIndex, int nTick, double* pdAccX, double* pdAccY)
{
	CDatabase* pDatabase = CDatabase::GetInstance();

	int nNumAdjVehicles = pDatabase->GetNumAdjacentVehicles();

	double dForceX = 0.0;
	double dForceY = 0.0;

	//! Own vehicleの位置を計算する．
	double dOwnPosX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_POSX);
	double dOwnPosY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_POSY);
	double dOwnVelX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_VELX);
	double dOwnVelY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_VELY);
	int nOwnLane = (dOwnPosY - 0.5*LANE_WIDTH) / LANE_WIDTH;


	double dLanePotential = calcLanePotential(dOwnPosY);
	double dRoadPotential = calcRoadPotential(dOwnPosY);
	double dCarPotential = calcCarPotential(nIndex, nTick, dOwnPosX, dOwnPosY, dOwnVelX, dOwnVelY);
	
	double dLanePotential_dash = calcLanePotential(dOwnPosY + m_dDelta);
	double dRoadPotential_dash = calcRoadPotential(dOwnPosY + m_dDelta);
	double dCarPotential_dash = calcCarPotential(nIndex, nTick, dOwnPosX, dOwnPosY + m_dDelta, dOwnVelX, dOwnVelY);

	dForceY = -(dLanePotential_dash - dLanePotential) - (dRoadPotential_dash - dRoadPotential) - (dCarPotential_dash - dCarPotential);
	dForceY /= m_dDelta;
	
	double dVelocityPotential = calcVelocityPotential(dOwnPosX, dOwnVelX, m_dVelDes);

	dCarPotential_dash = calcCarPotential(nIndex, nTick, dOwnPosX + m_dDelta, dOwnPosY, dOwnVelX, dOwnVelY);
	double dVelocityPotential_dash = calcVelocityPotential(dOwnPosX + m_dDelta, dOwnVelX, m_dVelDes);

	dForceX = -(dCarPotential_dash - dCarPotential) - (dVelocityPotential_dash - dVelocityPotential);
	dForceX /= m_dDelta;
	


	//! 求まった加速度を記録する
	*pdAccX = dForceX;
	*pdAccY = dForceY;
}
/***************************************************************************************/
/* Private functions */
/***************************************************************************************/
double CWolfPF::calcLanePotential(double dOwnPosY)
{
	double dPotential = 0.0;

	double dPotentialFromLeftLine = 0.0;
	double dPotentialFromRightLine = 0.0;

	double A_lane = 2.0;
	double sigma = 0.3 * LANE_WIDTH;

	double dY_left = 2 * LANE_WIDTH;
	double dY_right = LANE_WIDTH;


	dPotentialFromLeftLine = A_lane * qExp(-(dOwnPosY - dY_left)*(dOwnPosY - dY_left) / (2 * sigma*sigma));
	dPotentialFromRightLine = A_lane * qExp(-(dOwnPosY - dY_right)*(dOwnPosY - dY_right) / (2 * sigma*sigma));

	dPotential = dPotentialFromLeftLine + dPotentialFromRightLine;

	return dPotential;
}

double CWolfPF::calcRoadPotential(double dOwnPosY)
{
	double dPotential = 0.0;

	double dLeftRoadPos = 3 * LANE_WIDTH;
	double dRightRoadPos = 0.0;

	double eta = 3.0;

	double dPotentialFromLeftRoad = 0.5 * eta / qPow((dOwnPosY - dLeftRoadPos), 2);
	double dPotentialFromRightRoad = 0.5 * eta / qPow((dOwnPosY - dRightRoadPos), 2);

	dPotential = dPotentialFromLeftRoad + dPotentialFromRightRoad;

	return dPotential;
}

double CWolfPF::calcCarPotential(int nIndex, int nTick, double dOwnPosX, double dOwnPosY, double dOwnVelX, double dOwnVelY)
{
	double dPotential = 0.0;

	double A_car = 10.0;
	double alpha = 0.5;

	CDatabase* pDatabase = CDatabase::GetInstance();
	int nNumAdjVehicles = pDatabase->GetNumAdjacentVehicles();

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		if (n == nIndex)
			continue;

		double dPseudoDistanceK = calcPseudoDistanceK(n, nTick, dOwnPosX, dOwnPosY, dOwnVelX, dOwnVelY);

		dPotential += A_car * qExp(-alpha * dPseudoDistanceK) / dPseudoDistanceK;
	}

	return dPotential;
}

double CWolfPF::calcPseudoDistanceK(int nIndex, int nTick, double dOwnPosX, double dOwnPosY, double dOwnVelX, double dOwnVelY)
{
	double dPseudoDistanceK = 0.0;
	CDatabase* pDatabase = CDatabase::GetInstance();
	double d0 = 20.0;
	double Tf = 3.0;
	double beta = 0.6;
	
	double dPosX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_POSX);
	double dPosY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_POSY);
	double dVelX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_VELX);
	double dVelY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_VELY);
	
	if ((dPosX - dOwnPosX) < 0.0 || (dOwnVelX - dVelX) < 0.0)
	{
		dPseudoDistanceK = qSqrt((dPosX - dOwnPosX)*(dPosX - dOwnPosX) + (dPosY - dOwnPosY)*(dPosY - dOwnPosY));
	}
	else
	{
		double xi_0 = 1.0;

		if (dOwnVelX >= (d0 / Tf))
			xi_0 = d0 / Tf / dOwnVelX;

		double xi_m = xi_0 * qExp(-beta*(dOwnVelX - dVelX));

		double dX_dash = xi_m * qAbs(dOwnPosX-dPosX);

		dPseudoDistanceK = qSqrt(dX_dash*dX_dash + (dPosY - dOwnPosY)*(dPosY - dOwnPosY));
	}
	
	return dPseudoDistanceK;
}

double CWolfPF::calcVelocityPotential(double dOwnPosX, double dOwnVelX, double dVelDes)
{
	double dPotential = 0.0;
	double gamma = 0.2;

	dPotential = gamma * (dOwnVelX - dVelDes) * dOwnPosX;

	return dPotential;
}