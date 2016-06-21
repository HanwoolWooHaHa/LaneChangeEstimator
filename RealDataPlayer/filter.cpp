/**
* @file	filter.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/01/16
* @brief	this file is for the management of data files
*/
#include "filter.h"
#include "Library\Eigen\Core"

#include <qDebug.h>

CFilter::CFilter()
{
	for (int n = 0; n < ADJ_VEH_MAX; n++)
		for (int t = 0; t < T_MAX; t++)
			for (int k = 0; k < 4; k++)
				m_dP[n][t][k] = 1.0;
}

CFilter::~CFilter()
{	
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
void CFilter::MovingAverageFilter(int nTick, int nAdjVehicleIndex, int nFeatureIndex)
{
#if 0
	int nNumAdjVehicles = CDatabase::GetInstance()->GetNumAdjacentVehicles();
	int nDataLength = CDatabase::GetInstance()->GetDataLength();

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		double dSum = 0.0;
		int nNumSumData = 0;

		if (nTick < MOV_AVG_FLT_SPAN)
		{
			for( int dt=0; dt < nTick; dt++ )
			{
				double dFeatureValue = CDatabase::GetInstance()->GetFeatureData(n, dt, nFeatureIndex);

				if (isnan(dFeatureValue))
					continue;

				dSum += dFeatureValue;
				nNumSumData++;
			}
		}
		else
		{
			for (int dt = 0; dt < MOV_AVG_FLT_SPAN; dt++)
			{
				double dFeatureValue = CDatabase::GetInstance()->GetFeatureData(n, nTick - dt, nFeatureIndex);

				if (isnan(dFeatureValue))
					continue;

				dSum += dFeatureValue;
				nNumSumData++;
			}
		}

		if (nNumSumData != 0)
		{
			double dAvg = dSum / nNumSumData;

			if (qAbs(dAvg) > 3.0)
			{
				double dPreValue = CDatabase::GetInstance()->GetFeatureData(n, nTick - 1, nFeatureIndex);
				CDatabase::GetInstance()->SetFeatureData(n, nTick, nFeatureIndex, dPreValue);
			}
			else
			{
				CDatabase::GetInstance()->SetFeatureData(n, nTick, nFeatureIndex, dAvg);
			}
		}
	}
#endif
}

void CFilter::MovingAverageFilter(int nDataLength, int nNumAdjVehicles, void* pData)
{
	double(*pdData)[FILE_COLUMN_MAX] = reinterpret_cast<double(*)[FILE_COLUMN_MAX]>(pData);

	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		double dTemp[T_MAX][2] = { 0.0 }; // PosX, PosY
		int nSize = (MOV_AVG_FLT_SPAN - 1) * 0.5;
		int nColumn = n * 2 + 4;

		for (int t = 0; t < nDataLength; t++)
		{
			if (t < nSize)
			{
				double dSum[2] = { 0.0 };
				int nNumData = 0;

				for (int k = 0; k<(nSize + 1); k++)
				{
					double dPosX = pdData[t + k][nColumn];
					double dPosY = pdData[t + k][nColumn + 1];

					if (isnan(dPosX) || isnan(dPosY))
						continue;

					dSum[0] += dPosX;
					dSum[1] += dPosY;

					nNumData++;
				}

				for( int k=0; k<t; k++ )
				{
					double dPosX = pdData[k][nColumn];
					double dPosY = pdData[k][nColumn + 1];

					if (isnan(dPosX) || isnan(dPosY))
						continue;

					dSum[0] += dPosX;
					dSum[1] += dPosY;

					nNumData++;
				}

				if (nNumData == 0)
				{
					dTemp[t][0] = pdData[t][nColumn];
					dTemp[t][1] = pdData[t][nColumn + 1];
				}
				else
				{
					dTemp[t][0] = dSum[0] / nNumData;
					dTemp[t][1] = dSum[1] / nNumData;
				}
			}
			else if( t >= (nDataLength-nSize) )
			{
				double dSum[2] = { 0.0 };
				int nNumData = 0;

				for( int k=-nSize; k<1; k++ )
				{
					double dPosX = pdData[t + k][nColumn];
					double dPosY = pdData[t + k][nColumn + 1];

					if (isnan(dPosX) || isnan(dPosY))
						continue;

					dSum[0] += dPosX;
					dSum[1] += dPosY;

					nNumData++;
				}

				for( int k=t+1; k<nDataLength; k++ )
				{
					double dPosX = pdData[k][nColumn];
					double dPosY = pdData[k][nColumn + 1];

					if (isnan(dPosX) || isnan(dPosY))
						continue;

					dSum[0] += dPosX;
					dSum[1] += dPosY;

					nNumData++;
				}

				if (nNumData == 0)
				{
					dTemp[t][0] = pdData[t][nColumn];
					dTemp[t][1] = pdData[t][nColumn + 1];
				}
				else
				{
					dTemp[t][0] = dSum[0] / nNumData;
					dTemp[t][1] = dSum[1] / nNumData;
				}
			}
			else
			{
				double dSum[2] = { 0.0 };
				int nNumData = 0;

				for( int k=-nSize; k<(nSize+1); k++ )
				{
					double dPosX = pdData[t + k][nColumn];
					double dPosY = pdData[t + k][nColumn + 1];

					if (isnan(dPosX) || isnan(dPosY))
						continue;

					dSum[0] += dPosX;
					dSum[1] += dPosY;

					nNumData++;
				}

				if (nNumData == 0)
				{
					dTemp[t][0] = pdData[t][nColumn];
					dTemp[t][1] = pdData[t][nColumn + 1];
				}
				else
				{
					dTemp[t][0] = dSum[0] / nNumData;
					dTemp[t][1] = dSum[1] / nNumData;
				}
			}
		}

		for (int t = 0; t < nDataLength; t++)
		{
			pdData[t][nColumn] = dTemp[t][0];
			pdData[t][nColumn+1] = dTemp[t][1];
		}
	}
}

void CFilter::KalmanFilter(int nTick, int nNumAdjVehicles, void* pData, void* pFileData)
{
	if (nTick == 0)
		return;

	double(*pdData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>(pData);
	double(*pdFileData)[FILE_COLUMN_MAX] = reinterpret_cast<double(*)[FILE_COLUMN_MAX]>(pFileData);
	
	for (int n = 0; n < nNumAdjVehicles; n++)
	{
		Eigen::MatrixXf A = Eigen::MatrixXf::Zero(2, 2);
		A(0, 0) = 1.0;
		A(1, 1) = 1.0;

		Eigen::MatrixXf H = Eigen::MatrixXf::Zero(1, 2);
		H(0, 0) = 1.0;
		
		Eigen::MatrixXf Q = Eigen::MatrixXf::Zero(2, 2);
		Q(0, 0) = 0.001;
		Q(1, 1) = 0.001;

		Eigen::MatrixXf P = Eigen::MatrixXf::Ones(2, 2);
		P(0, 0) = m_dP[n][nTick - 1][0];
		P(0, 1) = m_dP[n][nTick - 1][1];
		P(1, 0) = m_dP[n][nTick - 1][2];
		P(1, 1) = m_dP[n][nTick - 1][3];

		double R = 0.1;

		
		double dDistance = pdData[n][nTick][FEATURE_PACKET_DISTANCE];
		double dLateralVelocity = pdData[n][nTick][FEATURE_PACKET_LAT_VELOCITY];

		double dPreDistance = pdData[n][nTick - 1][FEATURE_PACKET_DISTANCE];
		double dPreLateralVelocity = pdData[n][nTick - 1][FEATURE_PACKET_LAT_VELOCITY];

		if (isnan(dDistance) || isnan(dLateralVelocity) || isnan(dPreDistance) || isnan(dPreLateralVelocity))
		{
			continue;
		}

		double dt = pdFileData[nTick][PACK_TIME];
		A(0, 1) = dt;

		Eigen::MatrixXf X_pre = Eigen::MatrixXf::Zero(2, 1);
		X_pre(0, 0) = dPreDistance;
		X_pre(1, 0) = dPreLateralVelocity;
			
		Eigen::MatrixXf X_hat = A * X_pre;
		Eigen::MatrixXf P_hat = A*P*(A.transpose()) + Q;

		Eigen::MatrixXf Part = H * P_hat * H.transpose();
		double dVal = Part(0,0);
		dVal += R;

		Eigen::MatrixXf K = P_hat * H.transpose() / dVal;

		Eigen::MatrixXf Est = H * X_hat;
		double dZ = dDistance - Est(0,0);

		Eigen::MatrixXf X = X_hat + K * dZ;
		P = P_hat - K * H * P_hat;

		pdData[n][nTick][FEATURE_PACKET_DISTANCE] = X(0, 0);
		pdData[n][nTick][FEATURE_PACKET_LAT_VELOCITY] = X(1, 0);
		
		m_dP[n][nTick][0] = P(0, 0);
		m_dP[n][nTick][1] = P(0, 1);
		m_dP[n][nTick][2] = P(1, 0);
		m_dP[n][nTick][3] = P(1, 1);
	}
}

#if 0
int CFilter::MovingAverageFilter( int nData, int nNumData, void* pDataInfo, void* pData, int nColumn )
{
	switch( nData )
	{
	case TRAFFIC:
		movingAverageFilterForTrafficData( nNumData, pDataInfo, pData );
		break;

	case FEATURE:
		movingAverageFilterForFeatureData( nNumData, pDataInfo, pData, nColumn );
		break;

	default:
		return -1;
	}

	return 1;
}

int CFilter::MedianFilter( int nNumVehicles, void* pDataInfo, void* pData, int nColumn )
{
	int (*pnDataInfo)[5] = reinterpret_cast<int(*)[5]>(pDataInfo);
	double (*pFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>( pData );

	for( int n=0; n<nNumVehicles; n++ )
	{
		int nSize = ( MED_FLT_SPAN - 1 ) * 0.5;
		int nDataLength = pnDataInfo[n][DATA_INFO_PACKET_DATA_LENGTH];
		
		for( int t=0; t<nDataLength; t++ )
		{
			if( t < nSize )
            {}
			else if( t >= (nDataLength-nSize) )
            {
				double dTemp[T_MAX] = { 0.0 };
				int k = 0;
				for( int i=t-nSize; i<=t+nSize; i++ )
				{
					dTemp[k] = pFeatureData[n][i][nColumn];
					k++;
				}
				sort( MED_FLT_SPAN, dTemp );
				k = ( MED_FLT_SPAN - 1 ) * 0.5;
				
				pFeatureData[n][t][nColumn] = dTemp[k];
			}
			else
			{}
		}
	}

	return 1;
}
///////////////////////////////////////////////////////////////////////////
/* Private functions */
///////////////////////////////////////////////////////////////////////////
int CFilter::movingAverageFilterForTrafficData( int nNumData, void* pDataInfo, void* pData )
{
	int (*pnDataInfo)[5] = reinterpret_cast<int(*)[5]>(pDataInfo);
	double (*pRawData)[T_MAX][NUM_COLUMN] = reinterpret_cast<double(*)[T_MAX][NUM_COLUMN]>( pData );

	for( int n=0; n<nNumData; n++ )
	{
		int nSize = ( MOV_AVG_SPAN - 1 ) * 0.5;
		int nDataLength = pnDataInfo[n][DATA_INFO_PACKET_DATA_LENGTH];
		double dTemp[T_MAX][2] = { 0.0 }; // PosX, PosY

		for( int t=0; t<nDataLength; t++ )
		{
			if( t < nSize )
            {
				double dSum[2] = { 0.0 };

				for( int k=0; k<(nSize+1); k++ )
                {
                    dSum[0] += pRawData[n][t+k][DATA_PACKET_X];
                    dSum[1] += pRawData[n][t+k][DATA_PACKET_Y];
                }

                for( int k=0; k<t; k++ )
                {
                    dSum[0] += pRawData[n][k][DATA_PACKET_X];
                    dSum[1] += pRawData[n][k][DATA_PACKET_Y];
                }

                dTemp[t][0] = dSum[0] / (nSize+1+t);
                dTemp[t][1] = dSum[1] / (nSize+1+t);
			}
			else if( t >= (nDataLength-nSize) )
            {
                double dSum[2] = { 0.0 };

                for( int k=-nSize; k<1; k++ )
                {
                    dSum[0] += pRawData[n][t+k][DATA_PACKET_X];
                    dSum[1] += pRawData[n][t+k][DATA_PACKET_Y];
                }

                for( int k=t+1; k<nDataLength; k++ )
                {
                    dSum[0] += pRawData[n][k][DATA_PACKET_X];
                    dSum[1] += pRawData[n][k][DATA_PACKET_Y];
                }

                int delta = nDataLength - t - 1;

                dTemp[t][0] = dSum[0] / (nSize+1+delta);
                dTemp[t][1] = dSum[1] / (nSize+1+delta);
            }
            else
            {
                double dSum[2] = { 0.0 };

                for( int k=-nSize; k<(nSize+1); k++ )
                {
                    dSum[0] += pRawData[n][t+k][DATA_PACKET_X];
                    dSum[1] += pRawData[n][t+k][DATA_PACKET_Y];
                }

                dTemp[t][0] = dSum[0] / MOV_AVG_SPAN;
                dTemp[t][1] = dSum[1] / MOV_AVG_SPAN;
            }
		}

		for( int t=0; t<nDataLength; t++ )
		{
			pRawData[n][t][DATA_PACKET_X] = dTemp[t][0];
			pRawData[n][t][DATA_PACKET_Y] = dTemp[t][1];
		}
	}

	return 1;
}

int CFilter::movingAverageFilterForFeatureData( int nNumData, void* pDataInfo, void* pData, int nColumn )
{
	int (*pnDataInfo)[5] = reinterpret_cast<int(*)[5]>(pDataInfo);
	double (*pRawData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>( pData );

	for( int n=0; n<nNumData; n++ )
	{
		int nSize = ( MOV_AVG_SPAN - 1 ) * 0.5;
		int nDataLength = pnDataInfo[n][DATA_INFO_PACKET_DATA_LENGTH];
		double dTemp[T_MAX] = { 0.0 };

		for( int t=0; t<nDataLength; t++ )
		{
			if( t < nSize )
            {
				double dSum = 0.0;

				for( int k=0; k<(nSize+1); k++ )
                {
                    dSum += pRawData[n][t+k][nColumn];
                }

                for( int k=0; k<t; k++ )
                {
                    dSum += pRawData[n][k][nColumn];
                }

                dTemp[t] = dSum / (nSize+1+t);
			}
			else if( t >= (nDataLength-nSize) )
            {
                double dSum = 0.0;

                for( int k=-nSize; k<1; k++ )
                {
                    dSum += pRawData[n][t+k][nColumn];
                }

                for( int k=t+1; k<nDataLength; k++ )
                {
                    dSum += pRawData[n][k][nColumn];
                }

                int delta = nDataLength - t - 1;

                dTemp[t] = dSum / (nSize+1+delta);
            }
            else
            {
                double dSum = 0.0;

                for( int k=-nSize; k<(nSize+1); k++ )
                {
                    dSum += pRawData[n][t+k][nColumn];
                }

                dTemp[t] = dSum / MOV_AVG_SPAN;
            }
		}

		for( int t=0; t<nDataLength; t++ )
		{
			pRawData[n][t][nColumn] = dTemp[t];
		}
	}

	return 1;
}

void CFilter::sort( int num, double* pdData )
{
	double temp = 0.0;

	for (int i=0; i<num; ++i)
	{
		for (int j=i+1; j<num; ++j)
		{
			if ( pdData[i] > pdData[j] )
			{
				temp = pdData[i];

				pdData[i] = pdData[j];
				
				pdData[j] = temp;
			}
		}
	}
}
#endif