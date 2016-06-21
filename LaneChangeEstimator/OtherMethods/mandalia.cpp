#include "mandalia.h"
#include "lineExtractor.h"

#include <QDebug>

CMandalia::CMandalia()
{
}

int CMandalia::Extract( int nMode, int nType, int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo, void* pStateTransition  )
{
	if( FEATURE_VECTOR_DIMENSION != 4 )
	{
		qDebug() << "mandalia.cpp @ Please set the vector dimension to 4 !!";
		return -1;
	}

	double (*pdRawData)[T_MAX][NUM_COLUMN] = reinterpret_cast<double(*)[T_MAX][NUM_COLUMN]>(pRawData);
	double (*pdFeatureData)[T_MAX][4] = reinterpret_cast<double(*)[T_MAX][4]>(pFeaData);
	int (*pnLabelData)[T_MAX] = reinterpret_cast<int(*)[T_MAX]>(pLabData);
	int (*pnDataInfo)[5] = reinterpret_cast<int(*)[5]>(pDataInfo);

	for( int n=0; n<nNumData; n++ )
	{
		int nVehicleNo = pnDataInfo[n][DATA_INFO_PACKET_VEHICLE_NO];
		int nDataLength = pnDataInfo[n][DATA_INFO_PACKET_DATA_LENGTH];

		// 1. find the center line
		int nStartLane, nEndLane;
		int nCenterLine = 0;

		if( nType == CHANGING )
		{
			nStartLane = pdRawData[n][0][DATA_PACKET_LANE];
			for( int t=0; t<nDataLength; t++ )
			{
				if( pdRawData[n][t][DATA_PACKET_LANE] != nStartLane )
				{
					nEndLane = pdRawData[n][t][DATA_PACKET_LANE];
					break;
				}
			}
			nCenterLine = findCenterLine( nStartLane, nEndLane );
		}
		else // nType == KEEPING
		{
			nCenterLine = pdRawData[n][0][DATA_PACKET_LANE]-1;
			if( nCenterLine == 4 ) nCenterLine = 3;
		}
		////////////////////////////////////////////////////////////////////////

		// 2. calculate the distance w.r.t the center line
		int nInitSide = 0;
		CLineExtractor::GetInstance()->DistanceWrtLane( pdRawData[n][0][DATA_PACKET_X], pdRawData[n][0][DATA_PACKET_Y], nCenterLine, nInitSide );

		for( int t=0; t<nDataLength; t++ )
		{
			double dX = pdRawData[n][t][DATA_PACKET_X];
			double dY = pdRawData[n][t][DATA_PACKET_Y];

			int nSide = 0;
			double dDistance = CLineExtractor::GetInstance()->DistanceWrtLane( dX, dY, nCenterLine, nSide );

			if( (nInitSide*nSide) == -1 )
				dDistance *= -1;

			// Lane position 0
			pdFeatureData[n][t][0] = dDistance * FEET_TO_METER; // unit : meter
		}

		// 3. calculate the velocity w.r.t the center line
		double dTempVx[T_MAX] = { 0.0 };
		double dt = 10.0;
		for( int t=1; t<nDataLength; t++ ) // unit : m/s
		{
			dTempVx[t] = ( pdFeatureData[n][t][0] - pdFeatureData[n][t-1][0] ) * dt;
		}
		dTempVx[0] = dTempVx[1];

		// 4. calculate lane position
		for( int t=0; t<nDataLength; t++ )
		{
			double dVx = dTempVx[t]; // unit : m/s
			double dVy = pdRawData[n][t][DATA_PACKET_VEL] * FEET_TO_METER; // unit : m/s

			if( qAbs( dVy ) < 0.001 )
			{
				if( dVy >= 0.0 ) dVy = 0.001;
				else dVy = -0.001;
			}

			pdFeatureData[n][t][1] = 10.0 / dVy * dVx + pdFeatureData[n][t][0]; // Lane position 10
			pdFeatureData[n][t][2] = 20.0 / dVy * dVx + pdFeatureData[n][t][0]; // Lane position 20
			pdFeatureData[n][t][3] = 30.0 / dVy * dVx + pdFeatureData[n][t][0]; // Lane position 30
		}

		// 5. calculate var
		double dTemp[T_MAX][FEATURE_VECTOR_DIMENSION] = { 0.0 };
		for( int t=0; t<nDataLength; t++ )
		{
			if( t < MANDALIA_WINDOW_SIZE )
			{
				for( int k=0; k<4; k++ )
				{
					int nSize = t+1;
					double dAvg = 0.0;
					for( int i=0; i<nSize; i++ )
					{
						dAvg += pdFeatureData[n][t-i][k];
					}
					dAvg = dAvg / nSize;

					double dSum = 0.0;
					for( int i=0; i<nSize; i++ )
					{
						dSum += ( pdFeatureData[n][t-i][k] - dAvg ) * ( pdFeatureData[n][t-i][k] - dAvg );
					}

					dTemp[t][k] = dSum / nSize;
				}
			}
			else
			{
				for( int k=0; k<4; k++ )
				{
					double dAvg = 0.0;
					for( int i=0; i<MANDALIA_WINDOW_SIZE; i++ )
					{
						dAvg += pdFeatureData[n][t-i][k];
					}
					dAvg = dAvg / MANDALIA_WINDOW_SIZE;
				
					double dSum = 0.0;
					for( int i=0; i<MANDALIA_WINDOW_SIZE; i++ )
					{
						dSum += ( pdFeatureData[n][t-i][k] - dAvg ) * ( pdFeatureData[n][t-i][k] - dAvg );
					}

					dTemp[t][k] = dSum / MANDALIA_WINDOW_SIZE;
				}
			}
		}

		// 6. Copy Data
		for( int t=0; t<nDataLength; t++ )
		{
			for( int k=0; k<FEATURE_VECTOR_DIMENSION; k++ )
			{
				pdFeatureData[n][t][k] = dTemp[t][k];
			}
		}

		// write the label of data
		for( int t=0; t<nDataLength; t++ )
		{
			if( nType == CHANGING )
			{
				int nClass = pdRawData[n][t][DATA_PACKET_LABEL];

				if( nClass == 3) nClass = 2;
				if( nClass == 4 ) nClass = 1;
				
				pnLabelData[n][t] = nClass;
			}
			else
				pnLabelData[n][t] = LANE_KEEPING;
		}
	}

	qDebug() << " featureExtractor.cpp @ Extraction of features was completed";

	return 1;
}

int CMandalia::findCenterLine( int nStartLane, int nEndLane )
{
	int nReturn = -1;

	if( (nStartLane == 1 && nEndLane == 2) || (nStartLane == 2 && nEndLane == 1) )
	{
		nReturn = 0;
	}
	else if( (nStartLane == 2 && nEndLane == 3) || (nStartLane == 3 && nEndLane == 2) )
	{
		nReturn = 1;
	}
	else if( (nStartLane == 3 && nEndLane == 4) || (nStartLane == 4 && nEndLane == 3) )
	{
		nReturn = 2;
	}
	else if( (nStartLane == 4 && nEndLane == 5) || (nStartLane == 5 && nEndLane == 4) )
	{
		nReturn = 3;
	}
	else if( (nStartLane == 5 && nEndLane == 6) || (nStartLane == 6 && nEndLane == 5) )
	{
		nReturn = 4;
	}
	else
	{
		qDebug() << "featureExtractor.cpp @ Lane data has some problems";
	}

	return nReturn;
}