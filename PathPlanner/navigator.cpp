#include "navigator.h"
#include "navigationMethod.h"
#include "NavigationMethod/normalPF.h"
#include "NavigationMethod/wolf.h"
#include "database.h"

#include "stddef.h"

CNavigator::CNavigator(double dDeltaTs)
{
	m_pAdjustMethod = new CNormalPF();
	m_pNavigationMethod = new CWolfPF();

	m_dDeltaTs = dDeltaTs;
}

CNavigator::~CNavigator()
{
	if (m_pNavigationMethod != NULL)
		delete m_pNavigationMethod;
}

void CNavigator::Initialize( void )
{
	m_pNavigationMethod->Initialize();
}

void CNavigator::Update(int nTick)
{
	CDatabase* pDatabase = CDatabase::GetInstance();
	int nNumVehicles = pDatabase->GetNumAdjacentVehicles();

	for (int n = 0; n < nNumVehicles; n++)
	{
		double dAccX = 0.0;
		double dAccY = 0.0;

		double dOwnPosX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_POSX);
		double dOwnPosY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_POSY);
		double dOwnVelX = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_VELX);
		double dOwnVelY = pDatabase->GetData(CDatabase::SIMULATION, nTick - 1, n, PACKET_SIMUL_VELY);

		//! 位置を更新する
		dOwnPosX += dOwnVelX * m_dDeltaTs;
		dOwnPosY += dOwnVelY * m_dDeltaTs;

		pDatabase->SetData(CDatabase::SIMULATION, nTick, n, dOwnPosX, PACKET_SIMUL_POSX);
		pDatabase->SetData(CDatabase::SIMULATION, nTick, n, dOwnPosY, PACKET_SIMUL_POSY);


		//! 自車の経路計画を行う
		if (n==0)
			m_pNavigationMethod->CalculateAccelerate(n, nTick, &dAccX, &dAccY);
		else
			m_pAdjustMethod->CalculateAccelerate(n, nTick, &dAccX, &dAccY);

		//! 速度を更新する
		dOwnVelX += dAccX * m_dDeltaTs;
		dOwnVelY += dAccY * m_dDeltaTs;

		if (dOwnVelY > 0.8) dOwnVelY = 0.8;
		if (dOwnVelY < -0.8) dOwnVelY = -0.8;

		pDatabase->SetData(CDatabase::SIMULATION, nTick, n, dOwnVelX, PACKET_SIMUL_VELX);
		pDatabase->SetData(CDatabase::SIMULATION, nTick, n, dOwnVelY, PACKET_SIMUL_VELY);
	}
}