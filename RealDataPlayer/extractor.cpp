/**
* @file	extractor.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/05/09
* @brief	this class is implemented for the 
*/

#include "extractor.h"
#include "database.h"
#include "lineExtractor.h"
#include "Library\LibPF\potential.h"
#include "Library\Math\calcNormalCum.h"

#include <qmath.h>
#include <qdebug.h>
/*********************************************************************/
CExtractor::CExtractor()
{
	Initialize();

	m_pDatabase = CDatabase::GetInstance();
	m_pLineExtractor = CLineExtractor::GetInstance();
}

CExtractor::~CExtractor()
{
	delete m_pDatabase;
	delete m_pLineExtractor;

	int nNumMarks = m_pDatabase->GetNumMarks();

	deleteTransMarksDataMemory(nNumMarks);
}
/*********************************************************************/
/* Public functions */
void CExtractor::Extract(int nTick)
{
	//!  1. 現時刻における自車の位置を読み込む（世界座標系）
	double dGlobalOwnX = m_pDatabase->GetData(CDatabase::MEASUREMENT, nTick, PACKET_OWN_GLOBAL_X);
	double dGlobalOwnY = m_pDatabase->GetData(CDatabase::MEASUREMENT, nTick, PACKET_OWN_GLOBAL_Y);
	double dOwnAttitude = m_pDatabase->GetData(CDatabase::MEASUREMENT, nTick, PACKET_OWN_GLOBAL_ATT);
	int nCurrentLine = m_pDatabase->GetCurrentLane();


	//!  2. 白線の近似曲線を抽出する
	extractApproximateCurves(dGlobalOwnX, dGlobalOwnY, dOwnAttitude);


	//! 3. Preceding vehicleを探索する
	int nNumAdjVehicles = m_pDatabase->GetNumAdjacentVehicles();
	int nPrecedingVehicleIndex = findPrecedingVehicle(nTick, dGlobalOwnX, dGlobalOwnY, nNumAdjVehicles);
	double dPredictionRange = SENSING_RANGE;
	
	if (nPrecedingVehicleIndex != -1) //! Preceding vehicleがある場合，Prediction rangeを計算する
	{
		int nColumn = nPrecedingVehicleIndex * 2 + 4;
		dPredictionRange = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
	}


	//!  4. Driving featuresを抽出する
	int nNumTargetVehicles = 0;
	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		int nColumn = n * 2 + 4;
		double dPosX = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		double dPosY = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn+1);
		bool bPredictionRange = false;
		m_pDatabase->SetTargetOnOff(n, false);

		if (isnan(dPosX) && isnan(dPosY)) // 測定されたデータがあるか確認する
		{
			bPredictionRange = false;
		}
		//else if (dPosX < -SENSING_RANGE || dPosX > SENSING_RANGE) // 推定範囲内にあるかを確認する
		else if (dPosX > dPredictionRange || dPosX <= 0.0)
		{
			bPredictionRange = false;
		}
		else
		{
			int nLeftLineInOut = 0;
			int nRightLineInOut = 0;

			double dDistanceWrtLeftLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLine + 1, nLeftLineInOut);
			double dDistanceWrtRightLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLine, nRightLineInOut);

			nLeftLineInOut = -nLeftLineInOut;

			if (nLeftLineInOut == 1 && nRightLineInOut == -1)
			{
				if (dDistanceWrtLeftLine <= LANE_WIDTH)
				{
					m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE, dDistanceWrtLeftLine);
					bPredictionRange = true;
				}
				else
					bPredictionRange = false;
			}
			else if (nRightLineInOut == 1 && nLeftLineInOut == -1)
			{
				if (dDistanceWrtRightLine <= LANE_WIDTH)
				{
					m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE, dDistanceWrtRightLine);
					bPredictionRange = true;
				}
				else
					bPredictionRange = false;
			}
			else if (nLeftLineInOut == -1 && nRightLineInOut == -1)
			{
				//bPredictionRange = false;
				bPredictionRange = true;
				if (dDistanceWrtLeftLine <= dDistanceWrtRightLine)
					m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE, -dDistanceWrtLeftLine);
				else
					m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE, -dDistanceWrtRightLine);
			}
			else
			{
				qDebug() << "extractor.cpp @ Calculation of Distance w.r.t Line had some errors";
				bPredictionRange = false;
			}
		}
		
		//! 推定対象範囲外なら特徴量抽出を中止する
		if (bPredictionRange == false)
		{
			for (int k = 0; k < FEATURE_VECTOR_DIMENSION; k++)
				m_pDatabase->SetFeatureData(n, nTick, k, NAN);

			continue;
		}

		//!  4.2 白線距離から垂直速度を抽出する
		if (nTick == 0)
		{
			qDebug() << "extractor.cpp @ Time = 0 during the calculation of the vertical velocity";
			continue;
		}

		double dPrePosX = m_pDatabase->GetData(CDatabase::FILE, nTick - 1, nColumn);
		double dPrePosY = m_pDatabase->GetData(CDatabase::FILE, nTick - 1, nColumn + 1);

		if (isnan(dPrePosX) || isnan(dPrePosY)) // 垂直速度が計算できない場合
		{
			m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_LAT_VELOCITY, 0.0);

			continue;
		}
		else
		{
			double dt = m_pDatabase->GetData(CDatabase::FILE, nTick, PACK_TIME); // -m_pDatabase->GetData(CDatabase::FILE, nTick - 1, PACK_TIME);
			double dCurrentDistance = m_pDatabase->GetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE);
			double dPreDistance = m_pDatabase->GetFeatureData(n, nTick - 1, FEATURE_PACKET_DISTANCE) * STD_DISTANCE;

			double dLateralVelocity = (dCurrentDistance - dPreDistance) / dt;

			m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_LAT_VELOCITY, dLateralVelocity); // unit: [m/s]

			//! 推定対象車としてカウントする
			nNumTargetVehicles++;
			m_pDatabase->SetTargetOnOff(n, true);
		}

		//!  4.3 Potential Featureを抽出する
		if (FEATURE_VECTOR_DIMENSION >= 3)
		{
			//!  4.3.1 Lead vehicleを探す
			int nLeadVehicleIndex = findLeadVehicle(nTick, n, nNumAdjVehicles);
			
			//!  4.3.2 Rear vehicleを探す
			int nRearVehicleIndex = findRearVehicle(nTick, n, nNumAdjVehicles);

			//qDebug() << "Target : " << n << ",  Lead : " << nLeadVehicleIndex << ",  Rear : " << nRearVehicleIndex;

			double dAdjAmounts[12] = { 0.0 }; //! 相対量

			//!  4.3.3 相対量を計算する
			calculateAdjAmounts(nTick, nPrecedingVehicleIndex, n, nLeadVehicleIndex, nRearVehicleIndex, dAdjAmounts);

			//!  4.3.4 Potential featureを抽出する
			double dPotentialFeature = extractPotentialFeature(dAdjAmounts);

			//!  4.3.5 Databaseに記録する
			m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_POTENTIAL, dPotentialFeature);
		}
	}


	//!  4. Scaling & Filtering
	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		//!  4.1 Scalingを行う
		double dDistanceWrtLine = m_pDatabase->GetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE);
		if (isnan(dDistanceWrtLine)==false)
			m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_DISTANCE, dDistanceWrtLine / STD_DISTANCE);

		double dLateralVelocity = m_pDatabase->GetFeatureData(n, nTick, FEATURE_PACKET_LAT_VELOCITY);
		if (isnan(dLateralVelocity)==false)
			m_pDatabase->SetFeatureData(n, nTick, FEATURE_PACKET_LAT_VELOCITY, dLateralVelocity / STD_LATERAL_VELOCITY);
	}

	//m_pDatabase->FilteringByKalman(nTick);

	//! 推定対象車の数を記録する
	m_pDatabase->SetNumTargetVehicles(nNumTargetVehicles);
}

void CExtractor::Initialize(void)
{
	m_ppdLeftTransMarksData = NULL;
	m_ppdRightTransMarksData = NULL;
	m_ppdLeftApproximateMarksData = NULL;
	m_ppdRightApproximateMarksData = NULL;
}
/*********************************************************************/
/* Private member functions */
void CExtractor::extractApproximateCurves(double dGlobalOwnX, double dGlobalOwnY, double dOwnAttitude)
{
	int nNumMarks = m_pDatabase->GetNumMarks();
	int nCurrentLane = m_pDatabase->GetCurrentLane();
	int nNumLeftLineMarks = 0;
	int nNumRightLineMarks = 0;

	//!  2.1 座標変換される点群のメモリを確保する
	newTransMarksDataMemory(nNumMarks);

	//!  2.2 座標変換を行う（世界座標系 --> 車座標系）
	for (int n = 0; n < nNumMarks; n++)
	{
		double dGlobalMarkX = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane + 1, n, 0);
		double dGlobalMarkY = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane + 1, n, 1);

		double dDashX = dGlobalMarkX - dGlobalOwnX;
		double dDashY = dGlobalMarkY - dGlobalOwnY;

		double dRadAttitue = -qDegreesToRadians(dOwnAttitude);

		double dVehicleMarkX = dDashX *qCos(dRadAttitue) - dDashY * qSin(dRadAttitue);
		double dVehicleMarkY = dDashX *qSin(dRadAttitue) + dDashY * qCos(dRadAttitue);

		m_ppdLeftTransMarksData[n][0] = dVehicleMarkX;
		m_ppdLeftTransMarksData[n][1] = dVehicleMarkY;

		if (dVehicleMarkX > -SENSING_RANGE && dVehicleMarkX < SENSING_RANGE)
		{
			nNumLeftLineMarks++;
		}
	}
	for (int n = 0; n < nNumMarks; n++)
	{
		double dGlobalMarkX = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane, n, 0);
		double dGlobalMarkY = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane, n, 1);

		double dDashX = dGlobalMarkX - dGlobalOwnX;
		double dDashY = dGlobalMarkY - dGlobalOwnY;

		double dRadAttitue = -qDegreesToRadians(dOwnAttitude);

		double dVehicleMarkX = dDashX *qCos(dRadAttitue) - dDashY * qSin(dRadAttitue);
		double dVehicleMarkY = dDashX *qSin(dRadAttitue) + dDashY * qCos(dRadAttitue);

		m_ppdRightTransMarksData[n][0] = dVehicleMarkX;
		m_ppdRightTransMarksData[n][1] = dVehicleMarkY;

		if (dVehicleMarkX > -SENSING_RANGE && dVehicleMarkX < SENSING_RANGE)
		{
			nNumRightLineMarks++;
		}
	}

	m_ppdLeftApproximateMarksData = new double*[nNumLeftLineMarks];
	for (int n = 0; n < nNumLeftLineMarks; n++)
	{
		m_ppdLeftApproximateMarksData[n] = new double[2];
	}
	m_ppdRightApproximateMarksData = new double*[nNumRightLineMarks];
	for (int n = 0; n < nNumRightLineMarks; n++)
	{
		m_ppdRightApproximateMarksData[n] = new double[2];
	}

	nNumLeftLineMarks = nNumRightLineMarks = 0;
	for (int n = 0; n < nNumMarks; n++)
	{
		double dVehicleMarkX = m_ppdLeftTransMarksData[n][0];
		double dVehicleMarkY = m_ppdLeftTransMarksData[n][1];

		if (dVehicleMarkX > -SENSING_RANGE && dVehicleMarkX < SENSING_RANGE)
		{
			m_ppdLeftApproximateMarksData[nNumLeftLineMarks][0] = dVehicleMarkX;
			m_ppdLeftApproximateMarksData[nNumLeftLineMarks][1] = dVehicleMarkY;

			nNumLeftLineMarks++;
		}
	}
	for (int n = 0; n < nNumMarks; n++)
	{
		double dVehicleMarkX = m_ppdRightTransMarksData[n][0];
		double dVehicleMarkY = m_ppdRightTransMarksData[n][1];

		if (dVehicleMarkX > -SENSING_RANGE && dVehicleMarkX < SENSING_RANGE)
		{
			m_ppdRightApproximateMarksData[nNumRightLineMarks][0] = dVehicleMarkX;
			m_ppdRightApproximateMarksData[nNumRightLineMarks][1] = dVehicleMarkY;

			nNumRightLineMarks++;
		}
	}

	//!  2.3 近似曲線を抽出する
	double dLeftLineCoefficient[3] = { 0.0 };
	double dRightLineCoefficient[3] = { 0.0 };

	m_pLineExtractor->Extract(nCurrentLane + 1, nNumLeftLineMarks, m_ppdLeftApproximateMarksData, dLeftLineCoefficient);
	m_pLineExtractor->Extract(nCurrentLane, nNumRightLineMarks, m_ppdRightApproximateMarksData, dRightLineCoefficient);

	//qDebug() << "lineExtractor.cpp @ Left line" << " : a0=" << dLeftLineCoefficient[0] << ", a1=" << dLeftLineCoefficient[1] << ", a2=" << dLeftLineCoefficient[2];
	//qDebug() << "lineExtractor.cpp @ Right line" << " : a0=" << dRightLineCoefficient[0] << ", a1=" << dRightLineCoefficient[1] << ", a2=" << dRightLineCoefficient[2];

	//!  メモリを解除する
	for (int n = 0; n < nNumLeftLineMarks; ++n)
	{
		delete[] m_ppdLeftApproximateMarksData[n];
	}
	delete[] m_ppdLeftApproximateMarksData;
	m_ppdLeftApproximateMarksData = NULL;

	for (int n = 0; n < nNumRightLineMarks; ++n)
	{
		delete[] m_ppdRightApproximateMarksData[n];
	}
	delete[] m_ppdRightApproximateMarksData;
	m_ppdRightApproximateMarksData = NULL;
}

void CExtractor::newTransMarksDataMemory(int nNumMarks)
{
	if (m_ppdLeftTransMarksData == NULL)
	{
		m_ppdLeftTransMarksData = new double*[nNumMarks];

		for (int n = 0; n < nNumMarks; n++)
		{
			m_ppdLeftTransMarksData[n] = new double[2];
		}
	}
	if (m_ppdRightTransMarksData == NULL)
	{
		m_ppdRightTransMarksData = new double*[nNumMarks];

		for (int n = 0; n < nNumMarks; n++)
		{
			m_ppdRightTransMarksData[n] = new double[2];
		}
	}
}

void CExtractor::deleteTransMarksDataMemory(int nNumMarks)
{
	for (int n = 0; n < nNumMarks; ++n)
	{
		delete [] m_ppdLeftTransMarksData[n];
		delete [] m_ppdRightTransMarksData[n];
	}

	delete[] m_ppdLeftTransMarksData;
	delete[] m_ppdRightTransMarksData;

	m_ppdLeftTransMarksData = NULL;
	m_ppdRightTransMarksData = NULL;
}

int CExtractor::findPrecedingVehicle(int nTick, double dGlobalOwnX, double dGlobalOwnY, int nNumAdjVehicles)
{
	int nPrecedingVehicleIndex = -1;
	int nCurrentLine = m_pDatabase->GetCurrentLane();

	double dMin = DBL_MAX;
	int nMinIndex = -1;
	bool bFinding = false;

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		int nColumn = n * 2 + 4;
		double dPosX = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		double dPosY = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn + 1);

		if (isnan(dPosX) && isnan(dPosY)) // 測定されたデータがあるか確認する
		{
			bFinding = false;
		}
		else if (dPosX > SENSING_RANGE) // Sensing範囲内にあるかを確認する
		{
			bFinding = false;
		}
		else
		{
			int nLeftLineInOut = 0;
			int nRightLineInOut = 0;

			double dDistanceWrtLeftLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLine + 1, nLeftLineInOut);
			double dDistanceWrtRightLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLine, nRightLineInOut);

			nLeftLineInOut = -nLeftLineInOut;

			if (nLeftLineInOut == -1 && nRightLineInOut == -1) // 自車と同じ車線を走っている
			{
				bFinding = true;

				if (dPosX < dMin)
				{
					dMin = dPosX;
					nMinIndex = n;
				}
			}
		}
	}

	if (bFinding)
		nPrecedingVehicleIndex = nMinIndex;

	return nPrecedingVehicleIndex;
}

int CExtractor::findLeadVehicle(int nTick, int nTargetIndex, int nNumAdjVehicles)
{
	int nLeadVehicleIndex = -1;
	int nCurrentLane = m_pDatabase->GetCurrentLane();
	
	int nTargetColumn = nTargetIndex * 2 + 4;
	double dTargetPosX = m_pDatabase->GetData(CDatabase::FILE, nTick, nTargetColumn);
	double dTargetPosY = m_pDatabase->GetData(CDatabase::FILE, nTick, nTargetColumn + 1);

	double dMin = DBL_MAX;

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		if (n == nTargetIndex)
			continue;

		int nColumn = n * 2 + 4;
		double dPosX = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		double dPosY = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn + 1);

		if (isnan(dPosX) || isnan(dPosY)) continue;

		//! 推定対象車と同じサイドにいるかをチェックする
		if (dTargetPosY >= 0.0 && dPosY <= 0.0)
			continue;
		if (dTargetPosY <= 0.0 && dPosY >= 0.0)
			continue;

		//! 推定対象車と同じ車線を走っているかをチェックする
		int nLineInOut = 0;
		double dDistanceWrtLine = 0.0;
		if (dTargetPosY > 0.0)
		{
			dDistanceWrtLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLane + 1, nLineInOut);

			nLineInOut = -nLineInOut;
		}
		else
		{
			dDistanceWrtLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLane, nLineInOut);
		}
		
		if (nLineInOut == -1) continue;
		if (dDistanceWrtLine > LANE_WIDTH) continue;

		//! 推定対象車との車間距離を求める
		double dGap = dPosX - dTargetPosX;

		//! 推定対象車より前方にいるかをチェックする
		if (dGap < 0.0) continue;

		//! 一番近い車を探す
		if (dGap < dMin)
		{
			dMin = dGap;
			nLeadVehicleIndex = n;
		}
	}

	return nLeadVehicleIndex;
}

int CExtractor::findRearVehicle(int nTick, int nTargetIndex, int nNumAdjVehicles)
{
	int nRearVehicleIndex = -1;
	int nCurrentLane = m_pDatabase->GetCurrentLane();

	int nTargetColumn = nTargetIndex * 2 + 4;
	double dTargetPosX = m_pDatabase->GetData(CDatabase::FILE, nTick, nTargetColumn);
	double dTargetPosY = m_pDatabase->GetData(CDatabase::FILE, nTick, nTargetColumn + 1);

	double dMin = DBL_MAX;

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		if (n == nTargetIndex)
			continue;

		int nColumn = n * 2 + 4;
		double dPosX = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		double dPosY = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn + 1);

		if (isnan(dPosX) || isnan(dPosY)) continue;

		//! 推定対象車と同じサイドにいるかをチェックする
		if (dTargetPosY >= 0.0 && dPosY <= 0.0)
			continue;
		if (dTargetPosY <= 0.0 && dPosY >= 0.0)
			continue;

		//! 推定対象車と同じ車線を走っているかをチェックする
		int nLineInOut = 0;
		double dDistanceWrtLine = 0.0;
		if (dTargetPosY > 0.0)
		{
			dDistanceWrtLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLane + 1, nLineInOut);

			nLineInOut = -nLineInOut;
		}
		else
		{
			dDistanceWrtLine = m_pLineExtractor->DistanceWrtLine(dPosX, dPosY, nCurrentLane, nLineInOut);
		}

		if (nLineInOut == -1) continue;
		if (dDistanceWrtLine > LANE_WIDTH) continue;

		//! 推定対象車との車間距離を求める
		double dGap = dTargetPosX - dPosX;

		//! 推定対象車より前方にいるかをチェックする
		if (dGap < 0.0) continue;

		//! 一番近い車を探す
		if (dGap < dMin)
		{
			dMin = dGap;
			nRearVehicleIndex = n;
		}
	}

	return nRearVehicleIndex;
}

bool CExtractor::calculateAdjAmounts(int nTick, int nPrecedingIndex, int nTargetIndex, int nLeadIndex, int nRearIndex, double* pdAdjAmounts)
{
	bool bReturn = false;

	//! 現時刻における位置
	double dPrecedingPos = 0.0;
	double dTargetPos = 0.0;
	double dLeadPos = 0.0;
	double dRearPos = 0.0;
	double dOwnPos = 0.0;

	//! 1 step 前における位置
	double dPrecedingPrePos = 0.0;
	double dTargetPrePos = 0.0;
	double dLeadPrePos = 0.0;
	double dRearPrePos = 0.0;
	double dOwnPrePos = 0.0;

	//! 1stepの時間幅
	double dt = m_pDatabase->GetData(CDatabase::FILE, nTick, PACK_TIME); // -m_pDatabase->GetData(CDatabase::FILE, nTick - 1, PACK_TIME);

	//! 現時刻における速度
	double dPrecedingVel = 0.0;
	double dTargetVel = 0.0;
	double dLeadVel = 0.0;
	double dRearVel = 0.0;
	double dOwnVel = 0.0;

	int nColumn = nTargetIndex * 2 + 4;
	dTargetPos = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
	dTargetPrePos = m_pDatabase->GetData(CDatabase::FILE, nTick - 1, nColumn);
	dTargetVel = (dTargetPos - dTargetPrePos) / dt;

	pdAdjAmounts[ADJ_AMOUNTS_OWN_IDX] = nTargetIndex;
	pdAdjAmounts[ADJ_AMOUNTS_OWN_GAP] = dTargetPos;
	pdAdjAmounts[ADJ_AMOUNTS_OWN_VEL] = -dTargetVel;
	
	if (nPrecedingIndex != -1)
	{
		nColumn = nPrecedingIndex * 2 + 4;
		dPrecedingPos = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		dPrecedingPrePos = m_pDatabase->GetData(CDatabase::FILE, nTick - 1, nColumn);
		dPrecedingVel = (dPrecedingPos - dPrecedingPrePos) / dt;

		pdAdjAmounts[ADJ_AMOUNTS_PRECED_IDX] = nPrecedingIndex;
		pdAdjAmounts[ADJ_AMOUNTS_PRECED_GAP] = dPrecedingPos - dTargetPos;
		pdAdjAmounts[ADJ_AMOUNTS_PRECED_VEL] = dPrecedingVel - dTargetVel;
	}
	else
	{
		pdAdjAmounts[ADJ_AMOUNTS_PRECED_IDX] = -1;
		pdAdjAmounts[ADJ_AMOUNTS_PRECED_GAP] = SENSING_RANGE;
		pdAdjAmounts[ADJ_AMOUNTS_PRECED_VEL] = 0.0;
	}

	if (nLeadIndex != -1)
	{
		nColumn = nLeadIndex * 2 + 4;
		dLeadPos = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		dLeadPrePos = m_pDatabase->GetData(CDatabase::FILE, nTick - 1, nColumn);
		dLeadVel = (dLeadPos - dLeadPrePos) / dt;

		pdAdjAmounts[ADJ_AMOUNTS_LEAD_IDX] = nLeadIndex;
		pdAdjAmounts[ADJ_AMOUNTS_LEAD_GAP] = dLeadPos - dTargetPos;
		pdAdjAmounts[ADJ_AMOUNTS_LEAD_VEL] = dLeadVel - dTargetVel;
	}
	else
	{
		pdAdjAmounts[ADJ_AMOUNTS_LEAD_IDX] = -1;
		pdAdjAmounts[ADJ_AMOUNTS_LEAD_GAP] = SENSING_RANGE;
		pdAdjAmounts[ADJ_AMOUNTS_LEAD_VEL] = 0.0;
	}

	if (nRearIndex != -1)
	{
		nColumn = nRearIndex * 2 + 4;
		dRearPos = m_pDatabase->GetData(CDatabase::FILE, nTick, nColumn);
		dRearPrePos = m_pDatabase->GetData(CDatabase::FILE, nTick - 1, nColumn);
		dRearVel = (dRearPos - dRearPrePos) / dt;

		pdAdjAmounts[ADJ_AMOUNTS_REAR_IDX] = nRearIndex;
		pdAdjAmounts[ADJ_AMOUNTS_REAR_GAP] = dRearPos - dTargetPos;
		pdAdjAmounts[ADJ_AMOUNTS_REAR_VEL] = dRearVel - dTargetVel;
	}
	else
	{
		pdAdjAmounts[ADJ_AMOUNTS_REAR_IDX] = -1;
		pdAdjAmounts[ADJ_AMOUNTS_REAR_GAP] = SENSING_RANGE;
		pdAdjAmounts[ADJ_AMOUNTS_REAR_VEL] = 0.0;
	}

	return bReturn;
}

double CExtractor::extractPotentialFeature(double* pdAdjAmounts)
{
	double dPotentialFeatureValue = 0.0;

	//!  1. 推定対象車が走行している車線におけるポテンシャルを計算する U_T
	double dRelVelForLead = pdAdjAmounts[ADJ_AMOUNTS_LEAD_VEL];
	double dRelVelForRear = pdAdjAmounts[ADJ_AMOUNTS_REAR_VEL];
	double dGapForLead = pdAdjAmounts[ADJ_AMOUNTS_LEAD_GAP];
	double dGapForRear = pdAdjAmounts[ADJ_AMOUNTS_REAR_GAP];
	
	if (isnan(dRelVelForLead))
		qDebug() << "extractor.cpp @ dRelVelForLead has some errors";
	if (isnan(dRelVelForRear))
		qDebug() << "extractor.cpp @ dRelVelForRear has some errors";
	if (isnan(dGapForLead))
		qDebug() << "extractor.cpp @ dGapForLead has some errors";
	if (isnan(dGapForRear))
		qDebug() << "extractor.cpp @ dGapForRear has some errors";

	double dPotentialWrtLead = CPotential::GetInstance()->Field(dGapForLead, dRelVelForLead, 180.0);
	double dPotentialWrtRear = CPotential::GetInstance()->Field(dGapForRear, dRelVelForRear, 0.0);


	//!  2. 自車が走行している車線におけるポテンシャルを計算する U_O
	double dRelVelForPreceding = pdAdjAmounts[ADJ_AMOUNTS_PRECED_VEL];
	double dRelVelForOwn = pdAdjAmounts[ADJ_AMOUNTS_OWN_VEL];
	double dGapForPreceding = pdAdjAmounts[ADJ_AMOUNTS_PRECED_GAP];
	double dGapForOwn = pdAdjAmounts[ADJ_AMOUNTS_OWN_GAP];

	if (isnan(dRelVelForPreceding))
		qDebug() << "extractor.cpp @ dRelVelForPreceding has some errors";
	if (isnan(dRelVelForOwn))
		qDebug() << "extractor.cpp @ dRelVelForOwn has some errors";
	if (isnan(dGapForPreceding))
		qDebug() << "extractor.cpp @ dGapForPreceding has some errors";
	if (isnan(dGapForOwn))
		qDebug() << "extractor.cpp @ dGapForOwn has some errors";

	double dPotentialWrtPreceding = CPotential::GetInstance()->Field(dGapForPreceding, dRelVelForPreceding, 180.0);
	double dPotentialWrtOwn = CPotential::GetInstance()->Field(dGapForOwn, dRelVelForOwn, 0.0);


	//!  3. 両車線におけるポテンシャルを計算する
	double dPotentialAtTargetLane = dPotentialWrtLead + dPotentialWrtRear;
	double dPotentialAtOwnLane = dPotentialWrtPreceding + dPotentialWrtOwn;

	double dRatio = 0.0;
	if (dPotentialAtOwnLane == 0.0 && dPotentialAtTargetLane != 0.0)
	{
		dPotentialFeatureValue = 1.0;
	}
	else if (dPotentialAtOwnLane == 0.0 && dPotentialAtTargetLane == 0.0)
	{
		dPotentialFeatureValue = 0.5;
	}
	else
	{
		dRatio = dPotentialAtTargetLane / dPotentialAtOwnLane;

		if (isnan(dRatio))
		{
			return 0.5;
		}

		if (dRatio >= 100.0)
		{
			dPotentialFeatureValue = 1.0;
		}
		else if (dRatio == 0.0)
		{
			dPotentialFeatureValue = 0.0;
		}
		else
		{
			dPotentialFeatureValue = CNormalCum::GetInstance()->CumNormal(qLn(dRatio));
		}
	}

	return dPotentialFeatureValue;
}