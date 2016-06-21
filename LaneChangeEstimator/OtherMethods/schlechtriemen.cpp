#include "schlechtriemen.h"
#include "lineExtractor.h"

#include <QDebug>
#include <qfile.h>
#include <qtextstream.h>
#include <qmath.h>

CSchlechtriemen::CSchlechtriemen()
{
}
///////////////////////////////////////////////////////////////////////////
/* Public functions */
///////////////////////////////////////////////////////////////////////////
int CSchlechtriemen::Extract( int nMode, int nType, int nNumData, void* pRawData, void* pFeaData, void* pLabData, void* pDataInfo, void* pStateTransition  )
{
	if( FEATURE_VECTOR_DIMENSION != 3 )
	{
		qDebug() << "schlechtriemen.cpp @ Please set the vector dimension to 3 !!";
		return -1;
	}

	double (*pdRawData)[T_MAX][NUM_COLUMN] = reinterpret_cast<double(*)[T_MAX][NUM_COLUMN]>(pRawData);
	double (*pdFeatureData)[T_MAX][FEATURE_VECTOR_DIMENSION] = reinterpret_cast<double(*)[T_MAX][FEATURE_VECTOR_DIMENSION]>(pFeaData);
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
			pdFeatureData[n][t][FEATURE_PACKET_DISTANCE] = dDistance * FEET_TO_METER; // unit : meter
		}

		// 3. calculate the velocity w.r.t the center line
		double dTempVx[T_MAX] = { 0.0 };
		double dt = 10.0;
		for( int t=1; t<nDataLength; t++ ) // unit : m/s
		{
			pdFeatureData[n][t][FEATURE_PACKET_LAT_VELOCITY] = ( pdFeatureData[n][t][FEATURE_PACKET_DISTANCE] - pdFeatureData[n][t-1][FEATURE_PACKET_DISTANCE] ) * dt;
		}
		pdFeatureData[n][0][FEATURE_PACKET_LAT_VELOCITY] = pdFeatureData[n][1][FEATURE_PACKET_LAT_VELOCITY];

		// 4. calculate the relative velocity w.r.t the preceding vehicle
		initializeAdjData();
		makeAdjacentVehicleList( nMode, n, pRawData, pDataInfo );

		qDebug() << " schlechtriemen.cpp @ Vehicle No." << nVehicleNo << "  - Adj. Vehicles : " << m_nNumAdjacentVehicles;

		for( int t=0; t<nDataLength; t++ )
		{
			double dGap[12] = { 0.0 }; // 0: lead vehicle no 1: gap lead 2: rel vel lead 3: lag vehicle no 4: gap lag 5: rel vel lag
				                                        // 6: preceding vehicle no 7: gap preceding 8: rel vel preceding 9:following vehicle no 10: gap following 11: rel vel following
			calculateVehicleGap( t, nMode, n, pRawData, pDataInfo, dGap );

			if( dGap[6] == 0.0 )
			{
				dGap[7] = SENSING_AREA + 0.1;
				dGap[8] = 0.0;
			}

			pdFeatureData[n][t][FEATURE_PACKET_REL_VELOCITY] = dGap[8] * FEET_TO_METER;
		}

		//// 5. Normalize Feature about the distance and the velocity
		//for (int t = 0; t<nDataLength; t++)
		//{
		//	pdFeatureData[n][t][FEATURE_PACKET_DISTANCE] = pdFeatureData[n][t][FEATURE_PACKET_DISTANCE] / STD_DISTANCE;
		//	pdFeatureData[n][t][FEATURE_PACKET_LAT_VELOCITY] = pdFeatureData[n][t][FEATURE_PACKET_LAT_VELOCITY] / STD_VELOCITY;
		//	pdFeatureData[n][t][FEATURE_PACKET_REL_VELOCITY] = pdFeatureData[n][t][FEATURE_PACKET_REL_VELOCITY] / STD_REL_VELOCITY;
		//}

		// write the label of data
		for( int t=0; t<nDataLength; t++ )
		{
			if( nType == CHANGING )
				pnLabelData[n][t] = pdRawData[n][t][DATA_PACKET_LABEL];
			else
				pnLabelData[n][t] = LANE_KEEPING;
		}
	}

	qDebug() << "schlechtriemen.cpp @ Extraction of features was completed";

	return 1;
}
///////////////////////////////////////////////////////////////////////////
/* Private member functions */
///////////////////////////////////////////////////////////////////////////
int CSchlechtriemen::findCenterLine( int nStartLane, int nEndLane )
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

int CSchlechtriemen::makeAdjacentVehicleList( int nMode, int nDataIndex, void* pRawData, void* pDataInfo )
{
	double (*pdRawData)[T_MAX][NUM_COLUMN] = reinterpret_cast<double(*)[T_MAX][NUM_COLUMN]>(pRawData);
	int (*pnDataInfo)[5] = reinterpret_cast<int(*)[5]>(pDataInfo);

	int nVehicleNo = pnDataInfo[nDataIndex][DATA_INFO_PACKET_VEHICLE_NO];
	int nDataLength = pnDataInfo[nDataIndex][DATA_INFO_PACKET_DATA_LENGTH];

	int nStartScene = pdRawData[nDataIndex][0][DATA_PACKET_SCENE];
	int nEndScene = pdRawData[nDataIndex][nDataLength-1][DATA_PACKET_SCENE];

	int nSearchingStartNo = nVehicleNo - 100;
	if( nSearchingStartNo < 0 ) nSearchingStartNo = 0;

	int nSearchingEndNo = nVehicleNo + 100;
	if( nSearchingEndNo > MAX_VEHICLE_NO ) nSearchingEndNo = MAX_VEHICLE_NO;

	QString path;
	QFile* pOpenFile;

	switch( nMode )
	{
	case TRAINER:
#if defined (US_101)
		path = "../../../Dataset/us-101/traffic/train_vehicle/";
#elif (I_80)
		path = "../../../Dataset/i-80/traffic/train_vehicle/";
#endif
		break;

	case TESTER:
#if defined (US_101)
		path = "../../../Dataset/us-101/traffic/test_vehicle/";
#elif (I_80)
		path = "../../../Dataset/i-80/traffic/test_vehicle/";
#endif
		break;
	}

	for( int num=nSearchingStartNo; num<nSearchingEndNo; num++ )
	{
		QString file = path + "no" + QString::number( num ) + ".csv";

		pOpenFile = new QFile( file );

		if( !pOpenFile->open(QIODevice::ReadOnly) )
        {
            delete pOpenFile;
            continue;
        }

		QTextStream in( pOpenFile );

		int nTime = 0;
		double dTemp[T_MAX][NUM_COLUMN] = { 0.0 };
        while( !in.atEnd() )
        {
            QString line = in.readLine();
            QStringList list = line.split(',');

            for( int j=0; j<list.size(); j++ )
            {
                dTemp[nTime][j] = list.at(j).toDouble();
            }

            nTime++;

			if( nTime >= T_MAX )
			{
				qDebug() << "featureExtractor.cpp @ data length wad over";
				return FAIL;
			}
        }

		bool flag = false;
		int nSceneGap = nStartScene - dTemp[0][DATA_PACKET_SCENE];

		for( int t=0; t<nTime; t++ )
		{
			if( dTemp[t][DATA_PACKET_SCENE] >= nStartScene && dTemp[t][DATA_PACKET_SCENE] <= nEndScene )
			{
				flag = true;
				break;
			}
		}

		if( flag == false )
		{
			delete pOpenFile;
			continue;
		}
		flag = false;

		//
		// Check distance with respect to the target vehicle
		//
		if( nSceneGap >= 0 )
		{
			int k=0;
			for( int t=nSceneGap; t<nTime; t++ )
			{
				int nScene1 = dTemp[t][DATA_PACKET_SCENE];
				int nScene2 = pdRawData[nDataIndex][k][DATA_PACKET_SCENE];

				if( nScene1 != nScene2 )
				{
					qDebug() << "Error : Scene is not synchronized,  no." << dTemp[0][0];
					delete pOpenFile;
					return FAIL;
				}

				int nLane1 = dTemp[t][DATA_PACKET_LANE];
				int nLane2 = pdRawData[nDataIndex][k][DATA_PACKET_LANE];
				int nDelta = qAbs( nLane1 - nLane2 );

				double dPosY1 = dTemp[t][DATA_PACKET_Y];
				double dPosY2 = pdRawData[nDataIndex][k][DATA_PACKET_Y];
				double dGap = qAbs( dPosY1 - dPosY2 );

				if( (nDelta==1) && (dGap*FEET_TO_METER < SENSING_AREA) ) //! distance limit is 50 m
				{
					flag = true;
					break;
				}

				k++;

				if( k >= nDataLength )
					break;
			}
		}
		else
		{
			int k=-nSceneGap;
			for( int t=0; t<nTime; t++ )
			{
				int nScene1 = dTemp[t][DATA_PACKET_SCENE];
				int nScene2 = pdRawData[nDataIndex][k][DATA_PACKET_SCENE];

				if( nScene1 != nScene2 )
				{
					qDebug() << "Error : Scene is not synchronized,  no." << dTemp[0][0];
					delete pOpenFile;
					return FAIL;
				}

				int nLane1 = dTemp[t][DATA_PACKET_LANE];
				int nLane2 = pdRawData[nDataIndex][k][DATA_PACKET_LANE];
				int nDelta = qAbs( nLane1 - nLane2 );

				double dPosY1 = dTemp[t][DATA_PACKET_Y];
				double dPosY2 = pdRawData[nDataIndex][k][DATA_PACKET_Y];
				double dGap = qAbs( dPosY1 - dPosY2 );

				if( (nDelta==1) && (dGap*FEET_TO_METER < SENSING_AREA) ) //! distance limit is 50 m
				{
					flag = true;
					break;
				}

				k++;

				if( k >= nDataLength )
					break;
			}
		}

		if( flag == false )
		{
			delete pOpenFile;
			continue;
		}

		if( nSceneGap >= 0 )
		{
			int k=0;
			for( int t=nSceneGap; t<nTime; t++ )
			{
				int nScene1 = dTemp[t][DATA_PACKET_SCENE];
				int nScene2 = pdRawData[nDataIndex][k][DATA_PACKET_SCENE];

				if( nScene1 != nScene2 )
				{
					qDebug() << "Error 2 : Scene is not synchronized,  no." << dTemp[0][0];
					delete pOpenFile;
					return FAIL;
				}

				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][k][ADJ_DATA_PACKET_VEHICLE_NO] = dTemp[0][DATA_PACKET_NO];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][k][ADJ_DATA_PACKET_SCENE] = dTemp[t][DATA_PACKET_SCENE];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][k][ADJ_DATA_PACKET_X] = dTemp[t][DATA_PACKET_X];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][k][ADJ_DATA_PACKET_Y] = dTemp[t][DATA_PACKET_Y];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][k][ADJ_DATA_PACKET_LANE] = dTemp[t][DATA_PACKET_LANE];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][k][ADJ_DATA_PACKET_VEL] = dTemp[t][DATA_PACKET_VEL];

				k++;

				if( k >= nDataLength )
					break;
			}

			for( int i=k; i<nDataLength; i++ )
			{
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][i][ADJ_DATA_PACKET_VEHICLE_NO] = dTemp[0][DATA_PACKET_NO];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][i][ADJ_DATA_PACKET_SCENE] = -1;
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][i][ADJ_DATA_PACKET_X] = -1;
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][i][ADJ_DATA_PACKET_Y] = -1;
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][i][ADJ_DATA_PACKET_LANE] = -1;
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][i][ADJ_DATA_PACKET_VEL] = -1;
			}
		}
		else
		{
			int k=nSceneGap;
			for( int t=0; t<nDataLength; t++ )
			{
				if( k < 0 || k >= nTime )
				{
					m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_VEHICLE_NO] = dTemp[0][DATA_PACKET_NO];
					m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_SCENE] = -1;
					m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_X] = -1;
					m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_Y] = -1;
					m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_LANE] = -1;
					m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_VEL] = -1;

					k++;
					continue;
				}

				int nScene1 = dTemp[k][DATA_PACKET_SCENE];
				int nScene2 = pdRawData[nDataIndex][t][DATA_PACKET_SCENE];

				if( nScene1 != nScene2 )
				{
					qDebug() << "Error 2 : Scene is not synchronized,  no." << dTemp[0][0];
					delete pOpenFile;
					return FAIL;
				}

				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_VEHICLE_NO] = dTemp[0][DATA_PACKET_NO];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_SCENE] = dTemp[k][DATA_PACKET_SCENE];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_X] = dTemp[k][DATA_PACKET_X];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_Y] = dTemp[k][DATA_PACKET_Y];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_LANE] = dTemp[k][DATA_PACKET_LANE];
				m_dAdjacentVehiclesData[m_nNumAdjacentVehicles][t][ADJ_DATA_PACKET_VEL] = dTemp[k][DATA_PACKET_VEL];

				k++;
			}
		}

		m_nAdjDataInfo[m_nNumAdjacentVehicles][0] = dTemp[0][DATA_PACKET_NO];
		m_nAdjDataInfo[m_nNumAdjacentVehicles][1] = nTime;
		m_nAdjDataInfo[m_nNumAdjacentVehicles][2] = dTemp[0][DATA_PACKET_LENGTH];
		m_nAdjDataInfo[m_nNumAdjacentVehicles][3] = dTemp[0][DATA_PACKET_WIDTH];

		m_nNumAdjacentVehicles++;

		delete pOpenFile;
	}

	return DONE;
}

void CSchlechtriemen::initializeAdjData( void )
{
	m_nNumAdjacentVehicles = 0;
	memset( m_nAdjDataInfo, 0, sizeof( int ) * MAX_ADJACENT * 10 );
	memset( m_dAdjacentVehiclesData, 0, sizeof( double ) * MAX_ADJACENT * T_MAX * NUM_ADJACENT_COLUMN );
}

int CSchlechtriemen::calculateVehicleGap( int nTime, int nMode, int nDataIndex, void* pRawData, void* pDataInfo, double* dGap )
{
	double (*pdRawData)[T_MAX][NUM_COLUMN] = reinterpret_cast<double(*)[T_MAX][NUM_COLUMN]>(pRawData);
	int (*pnDataInfo)[5] = reinterpret_cast<int(*)[5]>(pDataInfo);

	int nTgtNo = pdRawData[nDataIndex][nTime][DATA_PACKET_NO];
	double dTgtPosX = pdRawData[nDataIndex][nTime][DATA_PACKET_X];
	double dTgtPosY = pdRawData[nDataIndex][nTime][DATA_PACKET_Y];

	int nNumAdjVehicles = m_nNumAdjacentVehicles;

	double dMinGapForLead = DBL_MAX;
	double dMinGapForLag = DBL_MAX;
	double dMinGapForPreceding = DBL_MAX;
	double dMinGapForFollowing = DBL_MAX;
	double dDistance = 0.0;

	for( int n=0; n<nNumAdjVehicles; n++ )
	{
		int nScene = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_SCENE];
		double dAdjPosX = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_X];
		double dAdjPosY = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_Y];
		int nLane = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_LANE];

		if( nScene == -1 )
			continue;

		int nEndTime = pnDataInfo[nDataIndex][DATA_INFO_PACKET_DATA_LENGTH] - 1;
		int nTgtLane = pdRawData[nDataIndex][nEndTime][DATA_PACKET_LANE];
		int nCurrentLane = pdRawData[nDataIndex][nTime][DATA_PACKET_LANE];
		int nVehicleNo = m_nAdjDataInfo[n][0];

		if( nLane == nTgtLane )
		{
			if( dAdjPosY >= dTgtPosY ) // Lead vehicle
			{
				dDistance = qAbs( dAdjPosY - dTgtPosY );

				if( dDistance <= dMinGapForLead )
				{
					dMinGapForLead = dDistance;
					dGap[0] = m_nAdjDataInfo[n][0];
					dGap[1] = dDistance - 0.5 * ( m_nAdjDataInfo[n][2] + pdRawData[nDataIndex][nTime][DATA_PACKET_LENGTH] );
					dGap[2] = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_VEL] - pdRawData[nDataIndex][nTime][DATA_PACKET_VEL];
				}
			}
			else // Lag vehicle
			{
				dDistance = qAbs( dAdjPosY - dTgtPosY );

				if( dDistance <= dMinGapForLag )
				{
					dMinGapForLag = dDistance;
					dGap[3] = m_nAdjDataInfo[n][0];
					dGap[4] = dDistance - 0.5 * ( m_nAdjDataInfo[n][2] + pdRawData[nDataIndex][nTime][DATA_PACKET_LENGTH] );
					dGap[5] = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_VEL] - pdRawData[nDataIndex][nTime][DATA_PACKET_VEL];
				}
			}
		}
		else if( nLane == nCurrentLane )
		{
			if( dAdjPosY >= dTgtPosY ) // Preceding vehicle
			{
				dDistance = qAbs( dAdjPosY - dTgtPosY );

				if( dDistance <= dMinGapForPreceding )
				{
					dMinGapForPreceding = dDistance;
					dGap[6] = m_nAdjDataInfo[n][0];
					dGap[7] = dDistance - 0.5 * ( m_nAdjDataInfo[n][2] + pdRawData[nDataIndex][nTime][DATA_PACKET_LENGTH] );
					dGap[8] = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_VEL] - pdRawData[nDataIndex][nTime][DATA_PACKET_VEL];
				}
			}
			else // Following vehicle
			{
				dDistance = qAbs( dAdjPosY - dTgtPosY );

				if( dDistance <= dMinGapForFollowing )
				{
					dMinGapForFollowing = dDistance;
					dGap[9] = m_nAdjDataInfo[n][0];
					dGap[10] = dDistance - 0.5 * ( m_nAdjDataInfo[n][2] + pdRawData[nDataIndex][nTime][DATA_PACKET_LENGTH] );
					dGap[11] = m_dAdjacentVehiclesData[n][nTime][ADJ_DATA_PACKET_VEL] - pdRawData[nDataIndex][nTime][DATA_PACKET_VEL];
				}
			}
		}
	}

	return DONE;
}