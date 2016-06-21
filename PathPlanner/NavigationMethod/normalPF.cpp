#include "normalPF.h"
#include "../database.h"

#include <qmath.h>

CNormalPF::CNormalPF()
{

}

CNormalPF::~CNormalPF()
{

}

void CNormalPF::Initialize(void)
{

}

void CNormalPF::CalculateAccelerate(int nIndex, int nTick, double* pdAccX, double* pdAccY)
{
	CDatabase* pDatabase = CDatabase::GetInstance();

	int nNumAdjVehicles = pDatabase->GetNumAdjacentVehicles();

	//! For repulsive energy
	double dCoefficientG = 5.0;
	double dCoefficientO = 200.0;
	double dCorrelationG = 20.0;
	double dCorrelationO = 5.0;

	//! Own vehicleの位置を計算する．
	double dOwnPosX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_POSX);
	double dOwnPosY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_POSY);
	double dOwnVelX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_VELX);
	double dOwnVelY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, nIndex, PACKET_SIMUL_VELY);
	int nOwnLane = (dOwnPosY - 0.5*LANE_WIDTH) / LANE_WIDTH;

	
	//! 前後を走行する車両を検出する
	double dMinDistance = DBL_MAX;
	double dRelVelX = 0.0;

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		if (n == nIndex)
			continue;

		double dPosX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_POSX);
		double dPosY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_POSY);
		double dVelX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_VELX);
		double dVelY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_VELY);
		int nLane = (dPosY - 0.5*LANE_WIDTH) / LANE_WIDTH;

		//! 周辺車両が自車より後ろにいる場合は考慮しない
		if ((dPosX - dOwnPosX) < 0.0)
			continue;

		if (nLane != nOwnLane)
			continue;

		double dDistance = qAbs(dPosX - dOwnPosX);

		if (dDistance < dMinDistance)
		{
			dMinDistance = dDistance;
			dRelVelX = dVelX - dOwnVelX;
		}
	}

	double dDistanceOfGoal = 0.0;
	double dDistanceOfObstacle = 0.0;

	if (dMinDistance == DBL_MAX)
	{
		*pdAccX = 0.0;
		*pdAccY = 0.0;

		return;
	}
	else
	{
		dDistanceOfObstacle = dMinDistance;
		dDistanceOfGoal = dDistanceOfObstacle + 10.0;
	}

	double dForceX = (2 * dCoefficientO * dDistanceOfObstacle) / (dCorrelationO*dCorrelationO) * qExp(-dDistanceOfObstacle*dDistanceOfObstacle / (dCorrelationO*dCorrelationO)) * (1 - qExp(-dDistanceOfGoal*dDistanceOfGoal / (dCorrelationG*dCorrelationG)));
	double dForceX2 = dCoefficientO * qExp(-dDistanceOfObstacle*dDistanceOfObstacle / (dCorrelationO*dCorrelationO))*(-2 * dDistanceOfGoal / (dCorrelationG*dCorrelationG)*qExp(-dDistanceOfGoal*dDistanceOfGoal / (dCorrelationG*dCorrelationG)));
	double dForceX3 = -(2 * dCoefficientG*dDistanceOfGoal / (dCorrelationG*dCorrelationG))*qExp(-dDistanceOfGoal*dDistanceOfGoal / (dCorrelationG*dCorrelationG));

	double dPotential1 = calcPotential(nIndex, nTick, dOwnPosX, dOwnPosY);
	double dPotential2 = calcPotential(nIndex, nTick, dOwnPosX+0.001, dOwnPosY);

	double dCheck = -(dPotential2 - dPotential1) / 0.001;
	double dCheck2 = -(dForceX + dForceX2 + dForceX3);

	//! 求まった加速度を記録する
	*pdAccX = -(dForceX + dForceX2 + dForceX3);
	*pdAccY = 0.0;
}

double CNormalPF::calcPotential(int nIndex, int nTick, double dOwnPosX, double dOwnPosY)
{
	CDatabase* pDatabase = CDatabase::GetInstance();

	int nNumAdjVehicles = pDatabase->GetNumAdjacentVehicles();

	double dMinDistance = DBL_MAX;
	//! For repulsive energy
	double dCoefficientG = 5.0;
	double dCoefficientO = 200.0;
	double dCorrelationG = 20.0;
	double dCorrelationO = 5.0;

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		if (n == nIndex)
			continue;

		double dPosX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_POSX);
		double dPosY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_POSY);
		double dVelX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_VELX);
		double dVelY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_VELY);
		int nLane = (dPosY - 0.5*LANE_WIDTH) / LANE_WIDTH;

		//! 周辺車両が自車より後ろにいる場合は考慮しない
		if ((dPosX - dOwnPosX) < 0.0)
			continue;

		if (dOwnPosY != dPosY)
			continue;

		double dDistance = qAbs(dPosX - dOwnPosX);

		if (dDistance < dMinDistance)
		{
			dMinDistance = dDistance;
		}
	}

	double dDistanceOfGoal = 0.0;
	double dDistanceOfObstacle = 0.0;

	if (dMinDistance == DBL_MAX)
	{
		return 0.0;
	}
	else
	{
		dDistanceOfObstacle = dMinDistance;
		dDistanceOfGoal = dDistanceOfObstacle + 10.0;
	}

	double dPotential = dCoefficientO * qExp(-dDistanceOfObstacle*dDistanceOfObstacle / (dCorrelationO*dCorrelationO))*(1 - qExp(-dDistanceOfGoal*dDistanceOfGoal / (dCorrelationG*dCorrelationG)));
	dPotential -= dCoefficientG * qExp(-dDistanceOfGoal*dDistanceOfGoal / (dCorrelationG*dCorrelationG));
	dPotential += dCoefficientG;

	return dPotential;
}