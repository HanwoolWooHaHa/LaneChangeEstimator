/**
* @file	filter.cpp
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2016/01/16
* @brief	this file is for the management of data files
*/
#include "filter.h"
#include "defines.h"

#include <qDebug.h>

CFilter::CFilter()
{
}

CFilter::~CFilter()
{	
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
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