/**
* @file	estimator.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/04/26
* @brief	this class is implemented for the estimation algorithm using sensor information
*/

#include "estimator.h"
#include "database.h"
#include "mySVM.h"

#include <qmath.h>
#include <qdebug.h>
/*********************************************************************/
CEstimator::CEstimator(int nMethod)
{
	Initialize();

	m_pDatabase = CDatabase::GetInstance();

	switch (nMethod)
	{
	case SVM:
		m_pMethod = new CMySvm();
		break;

	case HMM:
		break;
	}
}

CEstimator::~CEstimator()
{
	delete m_pDatabase;
	delete m_pMethod;
}
/*********************************************************************/
/* Public functions */
void CEstimator::Estimate( int nTick )
{
	//!  1. ���ӎԗ��̐�
	int nNumAdjVehicles = m_pDatabase->GetNumAdjacentVehicles();

	//!  2. ���ӎԗ�����䂸���肵�Ă���
	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		//!  2.1 Prediction area�ɓ����Ă��邩���`�F�b�N����
		if (m_pDatabase->GetTargetOnOff(n) == false)
			continue;

		bool bEstimationResult = m_pMethod->Test(nTick, n);


	}

	
}

void CEstimator::Initialize( void )
{
	
}
/*********************************************************************/
/* Private member functions */
